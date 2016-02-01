
difference () {
    //Zylinderkopf Grundkörper
    cylinder (r=30, h=60);
    
    //Arretierungsflansch für Schneckenzylinder
    cylinder (r=28, h=5);
    
    //Mittelloch für Schnecke
    cylinder (r=9, h=62);
    
    // Bohrungen für M8 Schraube
    for (a=[1,3,5,6,7,9,11,13,14,15]) {
        rotate([0,0,a*22.5]) {
            translate ([45/2,0,0]) {
                cylinder (r=4.1, h=62);
            }
        }
    }   
}



