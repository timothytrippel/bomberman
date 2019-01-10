module basic_mux(input [1:0] a, input[1:0] b, input s, output [1:0] o);
	assign o = s ? a : b;
endmodule
