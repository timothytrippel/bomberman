module sub_module(
	input  [7:0] in,
	output [3:0] out
);

assign out = in[5:2];

endmodule

module nested_slicing(
	input  [7:0] in,
	output [1:0] out
);

sub_module sub(
	.in(in),
	.out(out[1:0])
);

endmodule
