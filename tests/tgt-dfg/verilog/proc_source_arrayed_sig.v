module proc_source_arrayed_sig(
	input clk,
	input rst,
	output reg [7:0] out
);	
	
	reg [1:0] mem [0:3];

	always @(posedge clk) begin
		if (rst) begin
			out <= 8'b00000000;
		end
		else begin
			out[1:0] <= mem[0];
			out[3:2] <= mem[1];
			out[5:4] <= mem[2];
			out[7:6] <= mem[3];
		end
	end

endmodule
