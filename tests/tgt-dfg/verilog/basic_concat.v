module basic_concat(input a, input b, input c, output [2:0] o);
	assign o = {c, b, a};
endmodule
