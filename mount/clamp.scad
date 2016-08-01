
$fn=32;

module teeth(v)
{
    translate([0,-0.75, 0] + v)
    hull()
    {
        translate([0,0.25, 1])
            cube([16,1,0.1]);
        cube([16, 1.5, 0.1]);
    }
}

module clamp_half()
{
    difference()
    {
        union()
        {
            translate([0,28/2,0.4])
                cube([16,5.5, 1.6]);

            translate([0,18,0.4])
                cube([16, 1.6, 5]);

            translate([0,0,10])
                cube([16,10.5, 1.6]);

            teeth([0,29.5/2,2]);
            
            translate([0, 18, 9])
                rotate(90, [0,1,0])
                    difference()
                    {
                        cylinder(r=9.5, h=16);
                        cylinder(r=7.9, h=16);
                        translate([-1, -10, 0])
                            #cube([9,10,18]);
                        translate([-1, -14, 0])
                            #cube([12,10,18]);
                    }
                    
            translate([11, 0, 11.5])
                    rotate(90, [1,0,0])
                        cylinder(r=1.5, h=10);
       }
       translate([16/2-2-0.5, 0,0])
            cylinder(r=4, h=20);
    }
}

mirror([0,1,0])
    clamp_half();
clamp_half();