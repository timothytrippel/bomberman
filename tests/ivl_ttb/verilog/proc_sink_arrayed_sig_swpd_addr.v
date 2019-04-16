module proc_sink_arrayed_sig_swpd_addr(
	input clk,
	input rst,
	input data_1,
	input data_2,
	input data_3,
	input data_4,
	output [7:0] out
);	
	
	reg [1:0] mem [3:0];

	always @(posedge clk) begin
		if (rst) begin
			mem[0] <= 2'b00;
			mem[1] <= 2'b00;
			mem[2] <= 2'b00;
			mem[3] <= 2'b00;
		end
		else begin
			mem[0] <= data_1;
			mem[1] <= data_2;
			mem[2] <= data_3;
			mem[3] <= data_4;
		end
	end

	assign out = {mem[0], mem[1], mem[2], mem[3]};

endmodule
