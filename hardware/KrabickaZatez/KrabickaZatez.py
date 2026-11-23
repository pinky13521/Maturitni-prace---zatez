import adsk.core
import adsk.fusion
import traceback
import math

MM = 0.1  # Fusion API pouziva vnitrne centimetry (1 mm = 0.1 cm)


def ensure_param(user_params, name, expr, unit, comment):
    existing = user_params.itemByName(name)
    if existing:
        return existing
    return user_params.add(name, adsk.core.ValueInput.createByString(expr), unit, comment)


def find_pcb_and_holes(root_comp):
    """Najde vlozene PCB v sestave, jeho bounding box a 4 M3 montazni diry."""
    bodies_with_xform = []

    for occ in root_comp.allOccurrences:
        name_l = occ.name.lower()
        if 'krabicka' in name_l or 'sloupky' in name_l:
            continue
        if occ.bRepBodies.count > 0:
            for b in occ.bRepBodies:
                bodies_with_xform.append((b, None))
        elif occ.component and occ.component.bRepBodies.count > 0:
            for b in occ.component.bRepBodies:
                bodies_with_xform.append((b, occ.transform2))

    if not bodies_with_xform:
        for b in root_comp.bRepBodies:
            bodies_with_xform.append((b, None))

    if not bodies_with_xform:
        return None, []

    min_x = min_y = min_z = float('inf')
    max_x = max_y = max_z = float('-inf')
    holes = []

    for b, xform in bodies_with_xform:
        bb = b.boundingBox
        if xform is not None:
            p_min = bb.minPoint.copy()
            p_max = bb.maxPoint.copy()
            p_min.transformBy(xform)
            p_max.transformBy(xform)
            b_min_x, b_max_x = min(p_min.x, p_max.x), max(p_min.x, p_max.x)
            b_min_y, b_max_y = min(p_min.y, p_max.y), max(p_min.y, p_max.y)
            b_min_z, b_max_z = min(p_min.z, p_max.z), max(p_min.z, p_max.z)
        else:
            b_min_x, b_max_x = bb.minPoint.x, bb.maxPoint.x
            b_min_y, b_max_y = bb.minPoint.y, bb.maxPoint.y
            b_min_z, b_max_z = bb.minPoint.z, bb.maxPoint.z

        min_x = min(min_x, b_min_x)
        min_y = min(min_y, b_min_y)
        min_z = min(min_z, b_min_z)
        max_x = max(max_x, b_max_x)
        max_y = max(max_y, b_max_y)
        max_z = max(max_z, b_max_z)

        for face in b.faces:
            geom = face.geometry
            if geom.surfaceType == adsk.core.SurfaceTypes.CylinderSurfaceType:
                cyl = adsk.core.Cylinder.cast(geom)
                axis = cyl.axis.copy()
                origin = cyl.origin.copy()
                if xform is not None:
                    axis.transformBy(xform)
                    origin.transformBy(xform)
                # M3 montazni dira v KiCadu ma prumer 3.2 mm -> polomer 1.6 mm = 0.16 cm
                if 0.145 <= cyl.radius <= 0.175 and abs(axis.z) > 0.98:
                    hx, hy = origin.x, origin.y
                    if not any(math.hypot(hx - ex, hy - ey) < 0.05 for ex, ey in holes):
                        holes.append((hx, hy))

    if len(holes) != 4:
        kicad_offsets = [
            (14.478 * MM, 4.318 * MM),
            (4.1148 * MM, 80.772 * MM),
            (57.404 * MM, 80.772 * MM),
            (57.404 * MM, 4.318 * MM),
        ]
        holes = [(min_x + dx, max_y - dy) for dx, dy in kicad_offsets]

    bbox = (min_x, min_y, min_z, max_x, max_y, max_z)
    return bbox, holes


def detect_pcb_sides(bbox, holes):
    """
    Z asymetricke ctvrte diry (14.48 mm od hrany svorek) automaticky urci:
    - na ktere Y hrane je vykonovy tranzistor a chladic (heatsink_at_max_y)
    - na ktere X hrane je 12V napajeci jack (jack_at_max_x)
    """
    min_x, min_y, _, max_x, max_y, _ = bbox
    # Najdeme diru, ktera je nejvic posunuta dovnitr v ose X (ta je u svorek a naproti chladici)
    shifted = max(holes, key=lambda h: min(abs(h[0] - min_x), abs(max_x - h[0])))
    # Chladic je na opacne Y hrane nez posunuta dira
    heatsink_at_max_y = abs(shifted[1] - min_y) < abs(max_y - shifted[1])
    # Napajeci jack je na opacne X hrane nez posunuta dira
    jack_at_max_x = abs(shifted[0] - min_x) < abs(max_x - shifted[0])
    return heatsink_at_max_y, jack_at_max_x


def add_rect(sketch, x0, y0, x1, y1):
    lines = sketch.sketchCurves.sketchLines
    lines.addTwoPointRectangle(
        adsk.core.Point3D.create(x0, y0, 0),
        adsk.core.Point3D.create(x1, y1, 0),
    )


def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        design = adsk.fusion.Design.cast(app.activeProduct)
        if not design:
            ui.messageBox('Otevri nejdriv Design ve Fusionu.')
            return

        root = design.rootComponent
        params = design.userParameters

        # Uzivatelske parametry (upravitelne v Modify -> Change Parameters)
        ensure_param(params, 'tloustka_dna', '3.0 mm', 'mm', 'Tloustka zakladni desky (dna)')
        ensure_param(params, 'vule_pcb', '4.0 mm', 'mm', 'Presah zakladni desky kolem PCB')
        ensure_param(params, 'vyska_sloupku', '8.0 mm', 'mm', 'Vyska valcu nad dnem (prostor pro piny a chlazeni)')
        ensure_param(params, 'chladic_sirka', '50.0 mm', 'mm', 'Sirka prostoru vedle tranzistoru pro chladic')
        ensure_param(params, 'sloupek_prumer', '8.5 mm', 'mm', 'Vnejsi prumer valce ze dna')
        ensure_param(params, 'vlozka_prumer', '4.0 mm', 'mm', 'Prumer diry pro M3 heat-set vlozku')
        ensure_param(params, 'vlozka_hloubka', '6.0 mm', 'mm', 'Hloubka diry pro heat-set vlozku')

        t_dna = params.itemByName('tloustka_dna').value
        vule = params.itemByName('vule_pcb').value
        v_sloupek = params.itemByName('vyska_sloupku').value
        chladic_w = params.itemByName('chladic_sirka').value
        sloupek_r = params.itemByName('sloupek_prumer').value / 2.0
        vlozka_r = params.itemByName('vlozka_prumer').value / 2.0

        bbox, holes = find_pcb_and_holes(root)
        if bbox is None:
            ui.messageBox(
                'V sestave nebylo nalezeno zadne PCB. Vloz nejdriv STEP model PCB do aktualniho designu.'
            )
            return

        min_x, min_y, min_z, max_x, max_y, _ = bbox
        heatsink_at_max_y, _ = detect_pcb_sides(bbox, holes)

        # Rozmery zakladni desky (dna): kolem PCB + misto pro chladic na strane tranzistoru
        out_x0 = min_x - vule
        out_x1 = max_x + vule
        if heatsink_at_max_y:
            out_y0 = min_y - vule
            out_y1 = max_y + chladic_w + vule
        else:
            out_y0 = min_y - chladic_w - vule
            out_y1 = max_y + vule

        occ_box = root.occurrences.addNewComponent(adsk.core.Matrix3D.create())
        comp = occ_box.component
        comp.name = 'Zakladna_PCB'

        planes = comp.constructionPlanes
        sketches = comp.sketches
        extrudes = comp.features.extrudeFeatures

        # 1) Vnejsi rovina dna: z = min_z - v_sloupek - t_dna
        z_bottom_out = min_z - v_sloupek - t_dna
        p_in_bottom = planes.createInput()
        p_in_bottom.setByOffset(comp.xYConstructionPlane, adsk.core.ValueInput.createByReal(z_bottom_out))
        plane_bottom = planes.add(p_in_bottom)
        plane_bottom.name = 'Rovina_Dno_Vnejsi'

        # Skica 1: Zakladni deska (dno bez sten)
        sk_dno = sketches.add(plane_bottom)
        sk_dno.name = 'Skica_Dno'
        add_rect(sk_dno, out_x0, out_y0, out_x1, out_y1)
        ext_dno_in = extrudes.createInput(
            sk_dno.profiles.item(0),
            adsk.fusion.FeatureOperations.NewBodyFeatureOperation,
        )
        ext_dno_in.setDistanceExtent(False, adsk.core.ValueInput.createByString('tloustka_dna'))
        ext_dno = extrudes.add(ext_dno_in)
        top_floor_face = ext_dno.endFaces.item(0)

        # 2) Skica 2: 4 valce ze dna pod montazni diry PCB
        sk_sloupky = sketches.add(top_floor_face)
        sk_sloupky.name = 'Skica_Valce'
        for hx, hy in holes:
            sk_sloupky.sketchCurves.sketchCircles.addByCenterRadius(
                adsk.core.Point3D.create(hx, hy, 0),
                sloupek_r,
            )

        sloupky_profs = adsk.core.ObjectCollection.create()
        for i in range(sk_sloupky.profiles.count):
            sloupky_profs.add(sk_sloupky.profiles.item(i))

        ext_sloupky_in = extrudes.createInput(
            sloupky_profs,
            adsk.fusion.FeatureOperations.JoinFeatureOperation,
        )
        ext_sloupky_in.setDistanceExtent(False, adsk.core.ValueInput.createByString('vyska_sloupku'))
        extrudes.add(ext_sloupky_in)

        # 3) Skica 3: Diry pro M3 heat-set vlozky shora valcu (z = min_z)
        p_in_pcb = planes.createInput()
        p_in_pcb.setByOffset(comp.xYConstructionPlane, adsk.core.ValueInput.createByReal(min_z))
        plane_pcb = planes.add(p_in_pcb)
        plane_pcb.name = 'Rovina_Spodek_PCB'

        sk_vlozky = sketches.add(plane_pcb)
        sk_vlozky.name = 'Skica_HeatSet_Vlozky'
        for hx, hy in holes:
            sk_vlozky.sketchCurves.sketchCircles.addByCenterRadius(
                adsk.core.Point3D.create(hx, hy, 0),
                vlozka_r,
            )

        vlozky_profs = adsk.core.ObjectCollection.create()
        for i in range(sk_vlozky.profiles.count):
            vlozky_profs.add(sk_vlozky.profiles.item(i))

        ext_vlozky_in = extrudes.createInput(
            vlozky_profs,
            adsk.fusion.FeatureOperations.CutFeatureOperation,
        )
        ext_vlozky_in.setDistanceExtent(False, adsk.core.ValueInput.createByString('-vlozka_hloubka'))
        extrudes.add(ext_vlozky_in)

        ui.messageBox(
            f'Hotovo! Vytvorena zakladni deska (dno bez sten) s mistem pro chladic a {len(holes)} valci pro M3 heat-set vlozky (dira 4.0 mm).\n\n'
            'Sirku pro chladic (chladic_sirka = 50 mm) a vysku valcu (vyska_sloupku = 8 mm) upravis v Modify -> Change Parameters.'
        )

    except Exception:
        if ui:
            ui.messageBox('Chyba ve skriptu:\n{}'.format(traceback.format_exc()))
