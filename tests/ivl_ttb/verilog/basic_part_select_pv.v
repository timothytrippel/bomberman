module basic_part_select_pv(input in, output [3:0] out);
	reg [1:0] vector_upper = 2'b11;
	assign out[3:2] = vector_upper;
endmodule
