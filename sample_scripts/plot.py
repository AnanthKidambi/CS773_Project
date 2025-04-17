import numpy as np
from matplotlib import pyplot as plt

stt_ticks = {
    'bc': 177861334500,
    'bfs': 150099052500,
    'cc_sv': 176906364500,
    'cc': 153113543500,
    'pr': 192120222500,
    'pr_spmv': 185612869000,
    'sssp': 215119787500,
    'tc': 558747144500,
}

dopp_ticks = {
    'bc': 172897435500,
    'bfs': 148376822500,
    'cc_sv': 169542968500,
    'cc': 151672603500,
    'pr': 190263693500,
    'pr_spmv': 184204465000,
    'sssp': 213111550500,
    'tc': 555792968500,
}

if __name__ == '__main__':
    stt_ticks_arr = [stt_ticks[gap] for gap in stt_ticks]
    dopp_ticks_arr = [dopp_ticks[gap] for gap in dopp_ticks]
    ratio = [
        stt_ticks[gap] / dopp_ticks[gap] for gap in stt_ticks
    ]
    print(f'Geomean: {np.prod(ratio) ** (1 / len(ratio))}')
    labels = list(stt_ticks.keys())
    plt.figure(figsize=(10, 6))
    plt.bar(labels, ratio, color='skyblue')
    plt.xlabel('Graph Algorithms')
    plt.ylabel('Ratio (DOPP / STT IPC)')
    plt.ylim(min(ratio) * 0.9, max(ratio) * 1.1)
    plt.title('Ratio of DOPP to STT IPC for Graph Algorithms')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.axhline(y=1, color='red', linestyle='--', linewidth=1)
    plt.savefig('plots/stt_dopp_ratio.png')