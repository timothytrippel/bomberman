///////////////////////////////////////////////////////////////////////////////
// File downloaded from http://www.nandland.com
///////////////////////////////////////////////////////////////////////////////
// Description: 
// A LFSR or Linear Feedback Shift Register is a quick and easy way to generate
// pseudo-random data inside of an FPGA.  The LFSR can be used for things like
// counters, test patterns, scrambling of data, and others.  This module
// creates an LFSR whose width gets set by a parameter.  The o_LFSR_Done will
// pulse once all combinations of the LFSR are complete.  The number of clock
// cycles that it takes o_LFSR_Done to pulse is equal to 2^g_Num_Bits-1.  For
// example setting g_Num_Bits to 5 means that o_LFSR_Done will pulse every
// 2^5-1 = 31 clock cycles.  o_LFSR_Data will change on each clock cycle that
// the module is enabled, which can be used if desired.
//
// Parameters:
// NUM_BITS - Set to the integer number of bits wide to create your LFSR.
///////////////////////////////////////////////////////////////////////////////
module lfsr(

    input i_Clk,
    input i_Enable,
 
     // Optional Seed Value
    input i_Seed_DV,
    input [NUM_BITS-1:0] i_Seed_Data,
 
    output [NUM_BITS-1:0] o_LFSR_Data,
    output o_LFSR_Done
);
    
    parameter NUM_BITS = 32;

    reg [NUM_BITS:1] r_LFSR = 0;
    reg              r_XNOR;

    // Purpose: Load up LFSR with Seed if Data Valid (DV) pulse is detected.
    // Othewise just run LFSR when enabled.
    always @(posedge i_Clk) begin
        if (i_Enable == 1'b1) begin
            if (i_Seed_DV == 1'b1)
                r_LFSR <= i_Seed_Data;
            else
                r_LFSR <= {r_LFSR[NUM_BITS-1:1], r_XNOR};
        end
    end
 
    // Create Feedback Polynomials.  Based on Application Note:
    // http://www.xilinx.com/support/documentation/application_notes/xapp052.pdf
    always @(*)
        begin
            case (NUM_BITS)
                3: begin
                    r_XNOR = r_LFSR[3] ^~ r_LFSR[2];
                end
                4: begin
                    r_XNOR = r_LFSR[4] ^~ r_LFSR[3];
                end
                5: begin
                    r_XNOR = r_LFSR[5] ^~ r_LFSR[3];
                end
                6: begin
                    r_XNOR = r_LFSR[6] ^~ r_LFSR[5];
                end
                7: begin
                    r_XNOR = r_LFSR[7] ^~ r_LFSR[6];
                end
                8: begin
                    r_XNOR = r_LFSR[8] ^~ r_LFSR[6] ^~ r_LFSR[5] ^~ r_LFSR[4];
                end
                9: begin
                    r_XNOR = r_LFSR[9] ^~ r_LFSR[5];
                end
                10: begin
                    r_XNOR = r_LFSR[10] ^~ r_LFSR[7];
                end
                11: begin
                    r_XNOR = r_LFSR[11] ^~ r_LFSR[9];
                end
                12: begin
                    r_XNOR = r_LFSR[12] ^~ r_LFSR[6] ^~ r_LFSR[4] ^~ r_LFSR[1];
                end
                13: begin
                    r_XNOR = r_LFSR[13] ^~ r_LFSR[4] ^~ r_LFSR[3] ^~ r_LFSR[1];
                end
                14: begin
                    r_XNOR = r_LFSR[14] ^~ r_LFSR[5] ^~ r_LFSR[3] ^~ r_LFSR[1];
                end
                15: begin
                    r_XNOR = r_LFSR[15] ^~ r_LFSR[14];
                end
                16: begin
                    r_XNOR = r_LFSR[16] ^~ r_LFSR[15] ^~ r_LFSR[13] ^~ r_LFSR[4];
                    end
                17: begin
                    r_XNOR = r_LFSR[17] ^~ r_LFSR[14];
                end
                18: begin
                    r_XNOR = r_LFSR[18] ^~ r_LFSR[11];
                end
                19: begin
                    r_XNOR = r_LFSR[19] ^~ r_LFSR[6] ^~ r_LFSR[2] ^~ r_LFSR[1];
                end
                20: begin
                    r_XNOR = r_LFSR[20] ^~ r_LFSR[17];
                end
                21: begin
                    r_XNOR = r_LFSR[21] ^~ r_LFSR[19];
                end
                22: begin
                    r_XNOR = r_LFSR[22] ^~ r_LFSR[21];
                end
                23: begin
                    r_XNOR = r_LFSR[23] ^~ r_LFSR[18];
                end
                24: begin
                    r_XNOR = r_LFSR[24] ^~ r_LFSR[23] ^~ r_LFSR[22] ^~ r_LFSR[17];
                end
                25: begin
                    r_XNOR = r_LFSR[25] ^~ r_LFSR[22];
                end
                26: begin
                    r_XNOR = r_LFSR[26] ^~ r_LFSR[6] ^~ r_LFSR[2] ^~ r_LFSR[1];
                end
                27: begin
                    r_XNOR = r_LFSR[27] ^~ r_LFSR[5] ^~ r_LFSR[2] ^~ r_LFSR[1];
                end
                28: begin
                    r_XNOR = r_LFSR[28] ^~ r_LFSR[25];
                end
                29: begin
                    r_XNOR = r_LFSR[29] ^~ r_LFSR[27];
                end
                30: begin
                    r_XNOR = r_LFSR[30] ^~ r_LFSR[6] ^~ r_LFSR[4] ^~ r_LFSR[1];
                end
                31: begin
                    r_XNOR = r_LFSR[31] ^~ r_LFSR[28];
                end
                32: begin
                    r_XNOR = r_LFSR[32] ^~ r_LFSR[22] ^~ r_LFSR[2] ^~ r_LFSR[1];
                end
                33: begin
                    r_XNOR = r_LFSR[33] ^~ r_LFSR[20];
                end
                34: begin
                    r_XNOR = r_LFSR[34] ^~ r_LFSR[27] ^~ r_LFSR[2] ^~ r_LFSR[1];
                end
                35: begin
                    r_XNOR = r_LFSR[35] ^~ r_LFSR[33];
                end
                36: begin
                    r_XNOR = r_LFSR[36] ^~ r_LFSR[25];
                end
                37: begin
                    r_XNOR = r_LFSR[37] ^~ r_LFSR[5] ^~ r_LFSR[4] ^~ r_LFSR[3] ^~ r_LFSR[2] ^~ r_LFSR[1];
                end
                38: begin
                    r_XNOR = r_LFSR[38] ^~ r_LFSR[6] ^~ r_LFSR[5] ^~ r_LFSR[1];
                end
                39: begin
                    r_XNOR = r_LFSR[39] ^~ r_LFSR[35];
                end
                40: begin
                    r_XNOR = r_LFSR[40] ^~ r_LFSR[38] ^~ r_LFSR[21] ^~ r_LFSR[19];
                end
                41: begin
                    r_XNOR = r_LFSR[41] ^~ r_LFSR[38];
                end
                42: begin
                    r_XNOR = r_LFSR[42] ^~ r_LFSR[41] ^~ r_LFSR[20] ^~ r_LFSR[19];
                end
                43: begin
                    r_XNOR = r_LFSR[43] ^~ r_LFSR[42] ^~ r_LFSR[38] ^~ r_LFSR[37];
                end
                44: begin
                    r_XNOR = r_LFSR[44] ^~ r_LFSR[43] ^~ r_LFSR[18] ^~ r_LFSR[17];
                end
                45: begin
                    r_XNOR = r_LFSR[45] ^~ r_LFSR[44] ^~ r_LFSR[42] ^~ r_LFSR[41];
                end
                46: begin
                    r_XNOR = r_LFSR[46] ^~ r_LFSR[45] ^~ r_LFSR[26] ^~ r_LFSR[25];
                end
                47: begin
                    r_XNOR = r_LFSR[47] ^~ r_LFSR[42];
                end
                48: begin
                    r_XNOR = r_LFSR[48] ^~ r_LFSR[47] ^~ r_LFSR[21] ^~ r_LFSR[20];
                end
                49: begin
                    r_XNOR = r_LFSR[49] ^~ r_LFSR[40];
                end
                50: begin
                    r_XNOR = r_LFSR[50] ^~ r_LFSR[49] ^~ r_LFSR[24] ^~ r_LFSR[23];
                end
                51: begin
                    r_XNOR = r_LFSR[51] ^~ r_LFSR[50] ^~ r_LFSR[36] ^~ r_LFSR[35];
                end
                52: begin
                    r_XNOR = r_LFSR[52] ^~ r_LFSR[49];
                end
                53: begin
                    r_XNOR = r_LFSR[53] ^~ r_LFSR[52] ^~ r_LFSR[38] ^~ r_LFSR[37];
                end
                54: begin
                    r_XNOR = r_LFSR[54] ^~ r_LFSR[53] ^~ r_LFSR[18] ^~ r_LFSR[17];
                end
                55: begin
                    r_XNOR = r_LFSR[55] ^~ r_LFSR[31];
                end
                56: begin
                    r_XNOR = r_LFSR[56] ^~ r_LFSR[55] ^~ r_LFSR[35] ^~ r_LFSR[34];
                end
                57: begin
                    r_XNOR = r_LFSR[57] ^~ r_LFSR[50];
                end
                58: begin
                    r_XNOR = r_LFSR[58] ^~ r_LFSR[39];
                end
                59: begin
                    r_XNOR = r_LFSR[59] ^~ r_LFSR[58] ^~ r_LFSR[38] ^~ r_LFSR[37];
                end
                60: begin
                    r_XNOR = r_LFSR[60] ^~ r_LFSR[59];
                end
                61: begin
                    r_XNOR = r_LFSR[61] ^~ r_LFSR[60] ^~ r_LFSR[46] ^~ r_LFSR[45];
                end
                62: begin
                    r_XNOR = r_LFSR[62] ^~ r_LFSR[61] ^~ r_LFSR[6] ^~ r_LFSR[5];
                end
                63: begin
                    r_XNOR = r_LFSR[63] ^~ r_LFSR[62];
                end
                64: begin
                    r_XNOR = r_LFSR[64] ^~ r_LFSR[63] ^~ r_LFSR[61] ^~ r_LFSR[60];
                end
                128: begin
                    r_XNOR = r_LFSR[128] ^~ r_LFSR[126] ^~ r_LFSR[101] ^~ r_LFSR[99];
                end 
            endcase // case (NUM_BITS)
        end // always @ (*)
 
 
    assign o_LFSR_Data = r_LFSR[NUM_BITS:1];
 
    // Conditional Assignment (?)
    assign o_LFSR_Done = (r_LFSR[NUM_BITS:1] == i_Seed_Data) ? 1'b1 : 1'b0;
 
endmodule // LFSR