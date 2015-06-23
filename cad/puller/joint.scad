thickness = 2.5;
dummy = 0.01;
$fn = 200;

module corner() {
	thickness_1 = 8;
	thread_1 = 8+0.2;
	//nut_1 = 13;
	//nut_1_h = 7;
	nut_1 = 0;
	nut_1_h = 0;

	thickness_2 = 8;
	thread_2 = 5+0.2;
	nut_2 = 7.9;
	nut_2_h = 4;

	thickness_3 = 8;
	thread_3 = 5+0.2;
	nut_3 = 7.9;
	nut_3_h = 4;

	distance = 20;

	a = 32;
	reinf_offset = 8;

	translate([thickness_2,thickness_3,thickness_1]) difference() {
		union() {
			difference() {
				cube([a,a,a]);
				difference() {
					translate([thickness,thickness,thickness]) cube([a,a,a]);
					translate([reinf_offset,0,0]) rotate([0,-45,0]) cube([thickness,a,a*sqrt(2)]);
					translate([0,reinf_offset,0]) rotate([90,-45,90]) cube([thickness,a,a*sqrt(2)]);
					translate([reinf_offset,0,0]) rotate([0,0,45]) cube([thickness,a,a*sqrt(2)]);
				}
			}
			translate([distance,distance,thickness+nut_1_h]) rotate([0,180,0])
				cylinder(r1=nut_1/2*sqrt(3/2)+thickness/2,r2=nut_1/2*sqrt(3/2)+nut_1_h,h=nut_1_h+dummy);

			translate([thickness+nut_2_h,distance,distance]) rotate([-90,0,90])
				cylinder(r1=nut_2/2*sqrt(3/2)+thickness/2,r2=nut_2/2*sqrt(3/2)+nut_2_h,h=nut_2_h+dummy);

			translate([distance,thickness+nut_3_h,distance]) rotate([90,0,0])
				cylinder(r1=nut_3/2*sqrt(3/2)+thickness/2,r2=nut_3/2*sqrt(3/2)+nut_3_h,h=nut_3_h+dummy);
		}
		union() {
			translate([distance,distance,-dummy]) {
				cylinder(r=thread_1/2,h=thickness+2*dummy);
				translate([0,0,thickness+dummy]) cylinder(r=nut_1/2*sqrt(3/2),h=nut_1_h+dummy,$fn=6);
			}
			translate([-dummy,distance,distance]) rotate([0,90,0]) {
				cylinder(r=thread_2/2,h=thickness+2*dummy);
				translate([0,0,thickness+dummy]) cylinder(r=nut_2/2*sqrt(3/2),h=nut_2_h+dummy,$fn=6);
			}
			translate([distance,-dummy,distance]) rotate([0,90,90]) {
				cylinder(r=thread_3/2,h=thickness+2*dummy);
				translate([0,0,thickness+dummy]) cylinder(r=nut_3/2*sqrt(3/2),h=nut_3_h+dummy,$fn=6);
			}
		}
	}
}

module angle() {
	thickness_1 = 8;
	thread_1 = 8+0.2;
	//nut_1 = 13;
	//nut_1_h = 7;
	nut_1 = 0;
	nut_1_h = 0;

	thickness_2 = 8;
	thread_2 = 4+0.2;
	nut_2 = 7.9;
	nut_2_h = 4;

	distance = 20;

	a = 32;
	reinf_offset = 8;

	translate([thickness_2,thickness_1,thickness_1]) difference() {
		union() {
			difference() {
				cube([a,a,a]);
				difference() {
					translate([thickness,-dummy,thickness]) cube([a,a+2*dummy,a]);
					translate([reinf_offset,0,0]) rotate([0,-45,0]) cube([thickness,a,a*sqrt(2)]);
				}
			}
			translate([distance,a/2,thickness+nut_1_h]) rotate([0,180,0])
				cylinder(r1=nut_1/2*sqrt(3/2)+thickness/2,r2=nut_1/2*sqrt(3/2)+nut_1_h,h=nut_1_h+dummy);

			translate([thickness+nut_2_h,a/2,distance]) rotate([-90,0,90])
				cylinder(r1=nut_2/2*sqrt(3/2)+thickness/2,r2=nut_2/2*sqrt(3/2)+nut_2_h,h=nut_2_h+dummy);
		}
		union() {
			translate([distance,a/2,-dummy]) {
				cylinder(r=thread_1/2,h=thickness+2*dummy);
				translate([0,0,thickness+dummy]) cylinder(r=nut_1/2*sqrt(3/2),h=nut_1_h+dummy,$fn=6);
			}
			translate([-dummy,a/2,distance]) rotate([0,90,0]) {
				cylinder(r=thread_2/2,h=thickness+2*dummy);
				translate([0,0,thickness+dummy]) cylinder(r=nut_2/2*sqrt(3/2),h=nut_2_h+dummy,$fn=6);
			}
		}
	}
}

//translate([8,8,8]) %cube([20,20,20]);
corner();
//angle();