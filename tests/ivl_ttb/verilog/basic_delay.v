module basic_delay(output reg [1:0] out);
	reg a;
	reg b;

	always
		out = #5 {a,b};
endmodule
