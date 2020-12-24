module sub_module(
	input        a,
	input        b,
	input  [1:0] c,
	output [3:0] out
);

assign out = {a,{c,b}};

endmodule

module nested_concats(
	input        a,
	input        b,
	input  [1:0] c,
	output       out_1,
	output [2:0] out_2
);

sub_module sub(
	.a(a),
	.b(b),
	.c(c),
	.out({out_2,out_1})
);

endmodule
