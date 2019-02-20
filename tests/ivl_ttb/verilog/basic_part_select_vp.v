module basic_part_select_vp(input in, output [1:0] out);
	reg [5:0] vector = 6'b110010;
	assign out = vector[2:1];
endmodule
