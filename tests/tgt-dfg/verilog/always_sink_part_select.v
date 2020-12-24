module always_sink_part_select(
	input clk, 
	input a, 
	output reg [1:0] out
);

	always @(posedge clk) begin
		out[1] <= a;
	end

endmodule