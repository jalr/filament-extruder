d_aussen = 92;
d_innen = 70;
hoehe = 30;

n_zapfen = 5;
breite_zapfen = 14.5;

dummy = 0.1;

$fn = 100;

difference() {
    union() {
        for(a=[0:360/n_zapfen:360]) {
            rotate([0,0,a]) {
                translate([0,-breite_zapfen/2,0])
                    cube([d_aussen/2, breite_zapfen, hoehe]);
            }
        }
        cylinder(r=d_innen/2, h=hoehe);
    }
    intersection() {
        translate([0,0,-dummy]) cylinder(r=(24*1.03)/2, h=hoehe+2*dummy);
        translate([-(16*1.04)/2, -(24*1.03)/2, -dummy]) cube([(16*1.04), (24*1.03), hoehe+2*dummy]);
    }
}