module pcb(scale = 1)
{
    scale([scale, scale, 1])
    {
        linear_extrude(height = 7)
           import(file = "tweety.dxf");
//        translate([26.3, 20.3, -20])
//            cylinder(r=0.1, h=30);   
    }
}

module isp(v)
{
    translate(v)
    {
        for(i=[-2.54:2.54:2.54])
        {
            translate([1.27,i,-10])
                cylinder(r=1, h=30, $fn=32);
            translate([-1.27,i,-10])
                cylinder(r=1, h=30, $fn=32);
        }
        
        
        translate([1.27 + 2.54*3, 0, -10])
            cylinder(r=2.8/2, h=30, $fn=32);

        translate([-1.27 - 2.54*3, 0, -10])
            cylinder(r=2.8/2, h=30, $fn=32);
    }
}

module clamp_mark(v)
{
    translate(v)
    {
        translate([14,-10,0])
            cube([2,20,1.5]);
        translate([-14-1.5,-10,0])
            cube([2,20,1.5]);        
    }
}

pcb_scale= 1.02;
isp_x = 26.3*pcb_scale;
isp_y = 20.3*pcb_scale;

module top()
{
    difference()
    {
        translate([12.4*pcb_scale-3, 5.9*pcb_scale-3, 0])
        cube([28.4*pcb_scale+2*3,50*pcb_scale+2*3, 6]);

        translate([0,0,3])
           pcb(pcb_scale);

        isp([isp_x, isp_y, 0]);
        clamp_mark([isp_x, isp_y, 0]);
        
        translate([isp_x, isp_y+22, 0])
            cylinder(r=12, h=10);
    }
}

module isp_support(v)
{
    translate(v)
    {
        difference()
        {
            translate([-24/2, -20/2, -9])
                cube([24, 20, 9]);
            isp();
        }
    }
}

module isp_support2(v)
{
    translate(v)
    {
        difference()
        {
            translate([-24/2, -20/2, 0])
                cube([24, 20, 15+1.8]);
 
            isp();           
            translate([-22/2, -13/2, 15])
                cube([22, 13, 1.8]);       
            translate([-6/2, -10/2, 0])
                cube([6, 10, 15+1.8]);
            
            translate([1.27 + 2.54*3, 0, -10])
                cylinder(r=3/2, h=30, $fn=32);

            translate([-1.27 - 2.54*3, 0, -10])
                cylinder(r=3/2, h=30, $fn=32);
        }
    }
}


top();
//isp_support([isp_x, isp_y, 0]);
//isp_support2([isp_x, isp_y, -15-9-1.8]);

