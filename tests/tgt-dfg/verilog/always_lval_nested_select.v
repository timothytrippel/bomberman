module always_lval_nested_select(
	input         clk,
	input  [3:0]  select,
	input  [15:0] data,
	output [31:0] out
);

reg [7:0] mem [3:0];

always @(posedge clk) begin
	mem[select[3:2]] <= data[15:08];
end

assign out = {mem[0], mem[1], mem[2], mem[3]};

endmodule
