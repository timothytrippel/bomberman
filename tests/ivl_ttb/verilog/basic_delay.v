module basic_delay(output reg out);
	reg a;

	always
		out = #5 a;
endmodule
