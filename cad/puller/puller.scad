thickness = 8;
width = 90;
dummy = 0.1;
$fn = 200;


/*
color("brown") {
	for(z=[0,width-thickness]) {
		translate([0,0,z]) {
			dxf_linear_extrude(file = "puller.dxf", layer="0", height = thickness, convexity=1);
		}
	}
}

color("grey") {
	translate([0,0,-10]) dxf_linear_extrude(file = "puller.dxf", layer="Bolts", height = width+20, convexity=1);
}

for(x=[105,270]) {
	for(y=[100,263]) {
		translate([x,y,width/2]) wheel();
	}
}


for(m=[0,1]) {
	for(n=[0,1]) {
		translate([117.5+m*165,270,n*(width-thickness/2)+(1-n)*thickness/2]) mirror([0,0,1-n]) rotate([90,90,180])  slider();
	}
}

*/
spring();

module slider() {
	// thickness of side part
	t = 8+0.4;
	
	// thickness of part
	tp = t+5;
	
	// width of notch
	w = 25;
	
	// width of part
	wp = w+16;
	
	axledia = 20 + 0.2;
	wheeldia = 160;
	
	hp = wheeldia/2+(tp-t)+axledia/2+t+3;
	// height of part
	//hp = 1.5*w;
	//echo(hp/2); -> 30
	
	dummy=0.01;
	$fn=200;
	
	translate([-tp/2,0,0]) difference() {
		union() {
			translate([dummy,0,0]) cube([tp-2*dummy,w,hp]);
			for(n=[0,1]) translate([n*(t+(tp-t)/2),-(wp-w)/2,0]) cube([(tp-t)/2,wp,hp]);	
		}
		union() {
			translate([(tp-t)/2,(tp-t)/2,hp-t]) cube([tp+2*dummy,w-(tp-t),t+dummy]);
			translate([(tp-t)/2,(tp-t)/2,(tp-t)+axledia]) hull() {
				cube([tp+2*dummy,w-(tp-t),hp-tp-axledia-(tp-t)/2-(w-(tp-t))/2]);
				translate([0,(w-(tp-t))/2,hp-tp-axledia-(tp-t)/2-(w-(tp-t))/2]) rotate([0,90,0]) cylinder(r=(w-(tp-t))/2,h=tp+2*dummy,w-(tp-t));
			}
			translate([(tp-t)/2,w/2,axledia/2+(tp-t)/2]) rotate([0,90,0]) cylinder(r=axledia/2,h=tp+2*dummy);		
		}
	}
	//%translate([-dummy+tp,w/2,axledia/2+(tp-t)]) rotate([0,90,0]) cylinder(r=wheeldia/2,h=tp+2*dummy);
}

module wheel() {
	color("orange") translate([0,0,-47/2]) difference() {
		cylinder(r=160/2, h=47);
		translate([0,0,-dummy]) cylinder(r=139/2, h=47+2*dummy);
	}
	color("lightgrey") {
		difference() {
			union() {
				difference() {
					translate([0,0,-47/2-dummy]) cylinder(r=139/2, h=47);
					translate([0,0,-47/2-2*dummy]) cylinder(r=132/2, h=7+2*dummy);
					translate([0,0,47/2-7]) cylinder(r=132/2, h=7+2*dummy);
				}
				translate([0,0,-60/2]) cylinder(r=75/2, h=60);
			}
			translate([0,0,-60/2-dummy]) cylinder(r=20/2, h=60+2*dummy);
		}
	}
}

module spring() {
	x=100;
	y=20;
	w=19;
	t=3;
	dummy=0.1;
	for (n=[-1,1]) {
		translate([n*(x/2-t),0,0]) cylinder(r=t, h=w);
	}
	difference() {
		scale([x/2-0.5*t,y]) cylinder(r=1, h=w, $fn=100);
		translate([0,0,-dummy]) scale([x/2-1.5*t,y-t]) cylinder(r=1, h=w+2*dummy);
		translate([-x/2,-y-dummy,-dummy]) cube([x,y+dummy,w+2*dummy]);
	}
	translate([0,y+5-t,w/2]) rotate([90,0,0]) difference() {
		cylinder(r=9/2+t, h=5);
		translate([0,0,-t]) cylinder(r=9/2, h=5);
	}
}
