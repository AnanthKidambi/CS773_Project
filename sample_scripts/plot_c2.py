import numpy as np
from matplotlib import pyplot as plt

stt_ticks = {
    'bc': 177861334500,
    'bfs': 150099052500,
    'cc_sv': 176906364500,
    'cc': 153113543500,
    'pr': 192120222500,
    'pr_spmv': 185612869000,
    # 'sssp': 215119787500,
    'tc': 558747144500,
}

dopp_stalling_oracle_ticks = {
    'bc': 172897435500,
    'bfs': 148376822500,
    'cc_sv': 169542968500,
    'cc': 151672603500,
    'pr': 190263693500,
    'pr_spmv': 184204465000,
    # 'sssp': 213111550500,
    'tc': 555792968500,
}

dopp_ip_ticks = {
    'bc': 168453404500,
    'bfs': 143955204500,
    'cc_sv': 173002649500,
    'cc': 149879453500,
    'pr': 191181445500,
    'pr_spmv': 183549464000,
    # 'sssp': -1,
    'tc': 549952281500,
}

dopp_cplx_ticks = {
    'bc': 168595235500,
    'bfs': 143715800500,
    'cc_sv': 172386775500,
    'cc': 149863897500,
    'pr': 195075669500,
    'pr_spmv': 182844479000,
    # 'sssp': 216444805500,
    'tc': 550215135500,
}

dopp_oracle_ticks = {
    'bc': 159024305500,
    'bfs': 136762914500,
    'cc_sv': 153077030500,
    'cc': 140666693500,
    'pr': 178360891500,
    'pr_spmv': 172560626000,
    # 'sssp': -1,
    'tc': 542032417500,
}

dopp_ip_acc = {
    'bc': 0.887494,
    'bfs': 0.806435,
    'cc_sv': 0.753939,
    'cc': 0.759849,
    'pr': 0.501395,
    'pr_spmv': 0.651175,
    # 'sssp': -1,
    'tc': 0.877102
}

dopp_cplx_acc = {
    'bc': 0.883330,
    'bfs': 0.810728,
    'cc_sv': 0.781152,
    'cc': 0.760902,
    'pr': 0.535705,
    'pr_spmv': 0.693305,
    # 'sssp': 0.756791,
    'tc': 0.877342
}

if __name__ == '__main__':
    stt_ticks_arr = [stt_ticks[gap] for gap in stt_ticks]
    dopp_ip_ticks_arr = [dopp_ip_ticks[gap] for gap in dopp_ip_ticks]
    dopp_cplx_ticks_arr = [dopp_cplx_ticks[gap] for gap in dopp_cplx_ticks]
    ip_speedup = stt_ticks_arr / np.array(dopp_ip_ticks_arr)
    cplx_speedup = stt_ticks_arr / np.array(dopp_cplx_ticks_arr)
    print(f'Geomean IP: {np.prod(ip_speedup) ** (1 / len(ip_speedup))}')
    print(f'Geomean CPLX: {np.prod(cplx_speedup) ** (1 / len(cplx_speedup))}')
    labels = list(stt_ticks.keys())
    plt.figure(figsize=(10, 6), dpi=500)
    x = np.arange(len(labels))  # the label locations
    width = 0.35  # the width of the bars
    plt.bar(x - width/2, ip_speedup, width, label='DOPP IPC', color='skyblue')
    plt.bar(x + width/2, cplx_speedup, width, label='DOPP CPLX', color='orange')
    plt.xticks(x, labels)
    plt.legend(['DOPP IP', 'DOPP CPLX'])
    plt.xlabel('Graph Algorithms')
    plt.ylabel('Speedup in IPC (DOPP/STT)')
    plt.ylim(min(np.concatenate([ip_speedup, cplx_speedup])) * 0.9, max(np.concatenate([ip_speedup, cplx_speedup])) * 1.1)
    plt.title('Speedup [with propagation] for Graph Algorithms')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.axhline(y=1, color='red', linestyle='--', linewidth=1)
    plt.savefig('plots/c2_speedup.png')
    
    ip_acc = [dopp_ip_acc[gap] for gap in dopp_ip_acc]
    cplx_acc = [dopp_cplx_acc[gap] for gap in dopp_cplx_acc]
    plt.figure(figsize=(10, 6), dpi=500)
    plt.bar(x - width/2, ip_acc, width, label='DOPP IP', color='skyblue')
    plt.bar(x + width/2, cplx_acc, width, label='DOPP CPLX', color='orange')
    plt.xticks(x, labels)
    plt.legend(['DOPP IP', 'DOPP CPLX'])
    plt.xlabel('Graph Algorithms')
    plt.ylabel('Accuracy')
    plt.ylim(0, 1.1)
    plt.title('Accuracy of Predictors for Graph Algorithms')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig('plots/c2_accuracy.png')
    
    # Plot the speedup of oracle wrt STT
    oracle_ticks_arr = [dopp_oracle_ticks[gap] for gap in dopp_oracle_ticks]
    oracle_speedup = stt_ticks_arr / np.array(oracle_ticks_arr)
    print(f'Geomean ORACLE: {np.prod(oracle_speedup) ** (1 / len(oracle_speedup))}')
    plt.figure(figsize=(10, 6), dpi=500)
    plt.bar(x, oracle_speedup, width, label='DOPP ORACLE', color='green')
    plt.xticks(x, labels)
    plt.legend(['DOPP ORACLE'])
    plt.xlabel('Graph Algorithms')
    plt.ylabel('Speedup in IPC(DOPP/STT)')
    plt.ylim(min(oracle_speedup) * 0.9, max(oracle_speedup) * 1.1)
    plt.title('Ratio of DOPP to STT IPC for Graph Algorithms')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.axhline(y=1, color='red', linestyle='--', linewidth=1)
    plt.savefig('plots/c2_oracle_speedup.png')
    
    # Filter cases with >70% accuracy for IP stride
    filtered_labels = [gap for gap in dopp_ip_acc if dopp_ip_acc[gap] > 0.75]
    filtered_stt_ticks = [stt_ticks[gap] for gap in filtered_labels]
    filtered_ip_ticks = [dopp_ip_ticks[gap] for gap in filtered_labels]
    filtered_oracle_ticks = [dopp_oracle_ticks[gap] for gap in filtered_labels]

    # Calculate speedups
    filtered_ip_speedup = np.array(filtered_stt_ticks) / np.array(filtered_ip_ticks)
    filtered_oracle_speedup = np.array(filtered_stt_ticks) / np.array(filtered_oracle_ticks)

    # Plot side-by-side bars
    x_filtered = np.arange(len(filtered_labels))
    plt.figure(figsize=(10, 6), dpi=500)
    width = 0.35
    plt.bar(x_filtered - width/2, filtered_ip_speedup, width, label='DOPP IP', color='skyblue')
    plt.bar(x_filtered + width/2, filtered_oracle_speedup, width, label='DOPP ORACLE', color='green')
    plt.xticks(x_filtered, filtered_labels)
    plt.legend(['DOPP IP', 'DOPP ORACLE'])
    plt.xlabel('Graph Algorithms')
    plt.ylabel('Speedup in IPC(DOPP/STT)')
    plt.ylim(min(np.concatenate([filtered_ip_speedup, filtered_oracle_speedup])) * 0.9,
             max(np.concatenate([filtered_ip_speedup, filtered_oracle_speedup])) * 1.1)
    plt.title('Speedup [with propagation] of DOPP IP Stride and ORACLE for Benchmarks with >75% Accuracy')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.axhline(y=1, color='red', linestyle='--', linewidth=1)

    # Add accuracy labels on the IP bars
    for i, acc in enumerate([dopp_ip_acc[gap] for gap in filtered_labels]):
        plt.text(x_filtered[i] - width/2, filtered_ip_speedup[i] + 0.005, f'acc=\n{acc*100:.2f}%', 
                 ha='center', va='bottom', fontsize=8, color='blue')

    plt.savefig('plots/c2_filtered_speedup.png')
    
    # Plot the speedups of stalling oracle and oracle in side-by-side bars
    stalling_oracle_ticks_arr = [dopp_stalling_oracle_ticks[gap] for gap in dopp_stalling_oracle_ticks]
    stalling_oracle_speedup = stt_ticks_arr / np.array(stalling_oracle_ticks_arr)

    plt.figure(figsize=(10, 6), dpi=500)
    plt.bar(x - width/2, stalling_oracle_speedup, width, label='DOPP Stalling Oracle', color='skyblue')
    plt.bar(x + width/2, oracle_speedup, width, label='DOPP Oracle', color='orange')
    plt.xticks(x, labels)
    plt.legend(['Without Propagation', 'With Propagation'])
    plt.xlabel('Graph Algorithms')
    plt.ylabel('Speedup in IPC (DOPP/STT)')
    plt.ylim(min(np.concatenate([stalling_oracle_speedup, oracle_speedup])) * 0.9,
             max(np.concatenate([stalling_oracle_speedup, oracle_speedup])) * 1.1)
    plt.title('Speedup of DOPP With/Without Propagation (using oracle predictor) for Graph Algorithms')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.axhline(y=1, color='red', linestyle='--', linewidth=1)
    plt.savefig('plots/c2_stalling_oracle_vs_oracle_speedup.png')