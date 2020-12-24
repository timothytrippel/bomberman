module cont_source_arrayed_sig_swpd_addr(
	output [7:0] out
);
	
	wire [1:0] in [3:0];

	assign out[1:0] = in[0];
	assign out[3:2] = in[1];
	assign out[5:4] = in[2];
	assign out[7:6] = in[3];

endmodule
