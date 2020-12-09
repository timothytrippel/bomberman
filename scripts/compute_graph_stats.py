import copy
import json

####################################################################
# Compute Maximum Fan-in --> Usenix20 Rebuttal
####################################################################
def compute_max_fanin(signals, dut_top_module, filename):
    total_inputs = 0
    total_sinks = 0
    max_fanin = 0
    local_fanins = []

    for sig in signals:
        num_inputs = len(signals[sig].conn)

        if dut_top_module in signals[sig].hierarchy and \
                not signals[sig].isinput and \
                num_inputs > 0:

            local_fanins.append(num_inputs)
            max_fanin = max(max_fanin, num_inputs)
            total_inputs += num_inputs
            total_sinks += 1

    print "Total Inputs   =", total_inputs
    print "Total Sinks    =", total_sinks
    print "Average Fan-in =", (float(total_inputs) / float(total_sinks))
    print "Max Fan-in     =", max_fanin
    json_dict = {"Fan-in": local_fanins}
    with open(filename, 'w') as jf:
        json.dump(json_dict, jf)
    jf.close()

def compute_max_bitwise_fanin(signals, dut_top_module, filename):
    total_num_input_bits = 0
    max_fanin = 0
    total_num_sink_bits = 0
    local_fanins = []

    for sig in signals:
        num_inputs = len(signals[sig].conn)
        sink_signal_width = signals[sig].width()
        num_input_bits = 0

        if dut_top_module in signals[sig].hierarchy and \
                not signals[sig].isinput and \
                num_inputs > 0:

            for c in signals[sig].conn:
                num_input_bits += (c.source_msb - c.source_lsb + 1)

            local_fanin = float(num_input_bits) / float(sink_signal_width)
            local_fanins.append(local_fanin)
            # if local_fanin > 100:
                # print signals[sig].fullname(), signals[sig].width()
                # print " Num Input Bits:   ", num_input_bits
                # print " Sink Signal Width:", sink_signal_width
                # for c in signals[sig].conn:
                    # print c
            max_fanin = max(max_fanin, local_fanin)
            total_num_input_bits += num_input_bits
            total_num_sink_bits += sink_signal_width

    print "Total Num Input Bits =", total_num_input_bits
    print "Total Num Sink Bits  =", total_num_sink_bits
    print "Average Fan-in       =", (float(total_num_input_bits) / float(total_num_sink_bits))
    print "Max Fan-in     =", max_fanin
    json_dict = {"Fan-in": local_fanins}
    with open(filename, 'w') as jf:
        json.dump(json_dict, jf)
    jf.close()

####################################################################
# DFS to find average path to other nodes --> Usenix20 Rebuttal
####################################################################
def compute_max_reg2reg_path(signals, dut_top_module, filename):
    # DFS on any signal to find average reg2reg path
    lengths = []
    def dfs(curr_sig, length):
        if curr_sig.isinput or curr_sig.isff:
            lengths.append(copy.deepcopy(length))
            # total_length += length
            # total_paths  += 1
            return

        for conn in curr_sig.conn:
            if not conn.source_sig.visited:
                conn.source_sig.visited = True
                dfs(conn.source_sig, length + 1)

        return

    # find inputs/regs
    for sig in signals:
        if dut_top_module in signals[sig].hierarchy and \
                (signals[sig].isinput or signals[sig].isff):

            signals[sig].visited = True
            for conn in signals[sig].conn:
                if not conn.source_sig.visited:
                    dfs(conn.source_sig, 1)

    for sig in signals:
        signals[sig].visited = False
    print "Total Path Length           =", sum(lengths)
    print "Total Paths                 =", len(lengths)
    print "Average Reg2Reg Path Length =", \
            (float(sum(lengths)) / float(len(lengths)))
    print "Max Reg2Reg Path Length     =", max(lengths)
    json_dict = {"Reg2Reg Path Length": lengths}
    with open(filename, 'w') as jf:
        json.dump(json_dict, jf)
    jf.close()
