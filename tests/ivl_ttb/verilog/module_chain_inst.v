module mod(
	input in,
	output out
);

assign out = in;

endmodule


module module_chain_inst(
	input  in,
	output out
);
	
	wire middle;

	mod mod_1(
		.in(in),
		.out(middle)
	);

	mod mod_2(
		.in(middle),
		.out(out)
	);

endmodule