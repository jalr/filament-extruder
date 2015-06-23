dummy=0.1;

inner = 50;
outer = 100;
width = 50;
height = 70;
upper_cut = 10;
thickness = 1.2;

/*
translate([0,0,0])
difference() {
	union() {
		rotate([0,45,0]) cube([outer,width,outer], center=true);
		translate([0,0,-outer/2]) cube([sqrt(2)*outer,width,outer], center=true);
	}
	rotate([0,45,0]) cube([inner,width+2*dummy,inner], center=true);
	translate([0,0,sqrt(2)*inner/2+sqrt(2)*(outer-inner)/4+sqrt(2)*dummy/2]) {
		rotate([0,45,0]) cube([(outer-inner)/2+dummy,width+2*dummy,(outer-inner)/2+dummy], center=true);
		cube([upper_cut,width+2*dummy,sqrt(2)*(outer-inner)], center=true);
	}
}
*/

module case() {
	difference() {
		shape(inner, outer, height, width, upper_cut, 0);
		translate([0,0,0])
			shape(inner+4*thickness, outer-4*thickness, height-2*thickness, width-2*thickness, upper_cut+4*thickness, 2*thickness);
		translate([0,width-thickness,0])
			shape(inner+2*thickness, outer-2*thickness, height-thickness, width, upper_cut+2*thickness, thickness);
	}
}

module lid() {
	clearance = 0.2;
	shape(inner+2*(thickness+clearance), outer-2*(thickness-clearance), height-(thickness+clearance), thickness, upper_cut+2*(thickness+clearance), thickness+clearance);
}

module shape(inner, outer, height, width, upper_cut, t=1) {
	rotate([90,0,0]) linear_extrude(height=width)
		polygon([
			[0,-inner*sqrt(2)/2],
			[inner*sqrt(2)/2,0],
			[upper_cut/2,inner*sqrt(2)/2-upper_cut/2], 
			[upper_cut/2,inner*sqrt(2)/2+upper_cut/2-2*t*sqrt(2)], 
			[(outer-inner)*sqrt(2)/4+sqrt(2)*t, outer*sqrt(2)/2-(outer-inner)/2*sqrt(2)/2-sqrt(2)*t],
			[outer*sqrt(2)/2, 0],
			[outer*sqrt(2)/2, -height],
////			[0, -height],
			[-outer*sqrt(2)/2, -height],
			[-outer*sqrt(2)/2, 0],
			[-(outer-inner)*sqrt(2)/4-sqrt(2)*t, outer*sqrt(2)/2-(outer-inner)/2*sqrt(2)/2-sqrt(2)*t],
			[-upper_cut/2,inner*sqrt(2)/2+upper_cut/2-2*t*sqrt(2)], 
			[-upper_cut/2,inner*sqrt(2)/2-upper_cut/2], 
			[-inner*sqrt(2)/2,0],
		]);
}

//lid();
case();
