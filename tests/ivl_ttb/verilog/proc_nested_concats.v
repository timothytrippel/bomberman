module sub_module(
	input            clk,
	input            a,
	input            b,
	input  [1:0]     c,
	output reg [3:0] out
);

always @(posedge clk) begin
	out <= {a,{c,{b}}};
end

endmodule

module proc_nested_concats(
	input        clk,
	input        a,
	input        b,
	input  [1:0] c,
	output       out_1,
	output [2:0] out_2
);

sub_module sub(
	.clk(clk),
	.a(a),
	.b(b),
	.c(c),
	.out({out_2,out_1})
);

endmodule
