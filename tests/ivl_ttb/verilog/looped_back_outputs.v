module submod(
	input in,
	output [7:0] out
);

assign out = 4'hDEAD;

endmodule

module looped_back_outputs(
	input in,
	output reg [31:0] out
);

	submod X (
		.in(in),
		.out(out[31:24]),
	)

	submod Y (
		.in(in),
		.out(out[7:0])
	)

endmodule