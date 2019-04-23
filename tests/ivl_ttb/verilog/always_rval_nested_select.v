module always_rval_nested_select(
	input            clk,
	input      [3:0] select, 
	output reg [7:0] out
);

wire [7:0] data [3:0];

always @(posedge clk) begin
	out <= data[select[1:0]];
end

endmodule
