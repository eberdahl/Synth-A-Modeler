// This DSP file has been generated by the Synth-A-Modeler compiler.
import("physicalmodeling.lib");

adjStiffness=hslider("stiffness", 2200.0, 500.0, 4000.0, 100.0);


bigBlock(m1p,m2p,m3p,gp,dev1p) = (m1,m2,m3,g,dev1,a1,a2) with {
	// For waveguide terminations:
	// For junctions:
	// For mass-like objects:
	m1 = (0.0-l1) : mass(0.01,0.0,0.0);
	m2 = (0.0+l1-l2-l3) : mass(0.01,0.0,0.0);
	m3 = (0.0+l2) : mass(0.03,0.0,0.0);
	dev1 = (0.0+l3);
	g = (0.0) : ground(0.0);

	// For link-like objects:
	l1 = (m1p - m2p) : link(2.0*adjStiffness,0.003,0.0);
	l2 = (m2p - m3p) : link(2.0*adjStiffness,0.004,0.0);
	l3 = (m2p - dev1p) : link(2.0*adjStiffness,0.005,0.0);

	// For audio outputs:
	a1 = m1*(1000.0)+l1*(100.0);
	a2 = l2*(1000.0)+l3*(-50.0)+l1*(0.01);
};

process = (bigBlock)~(_,_,_,_,!,!,!):(!,!,!,!,_,_,_);
