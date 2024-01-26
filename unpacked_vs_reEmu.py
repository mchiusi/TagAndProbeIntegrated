import uproot
import matplotlib.pyplot as plt
import awkward as ak
import argparse

def plot_histogram(variable, data_unpacked, data_reEmu, tag, bins=60): 
    bin_edges = plt.hist(data_unpacked, bins=bins, alpha=0)[1]

    plt.hist(data_unpacked, bins=bin_edges, alpha=0.5, color='blue', label='Unpacked')
    plt.hist(data_reEmu,    bins=bin_edges, alpha=0.5, color='red',  label='Re-Emulated')
    plt.ylabel("Counts")
    plt.xlabel(variable)
    if variable == 'tauEt': plt.yscale('log')
    plt.title("Unpacked vs re-emulated: "+ variable)
    plt.grid(True)
    plt.legend(loc='upper right')
    plt.savefig('plots/'+tag+'_'+variable+'.pdf', format='pdf')
    plt.clf()

def main():
    parser = argparse.ArgumentParser(description='Plot histograms from a ROOT file.')
    parser.add_argument('--file_path', type=str, default='L1Ntuple.root', help='Path to the ROOT file')
    parser.add_argument('--tag', type=str, default='no_tag', help='Tag to label the histograms')

    args = parser.parse_args()

    file_path = args.file_path
    
    tree_name_unpacked = 'l1UpgradeTree/L1UpgradeTree/L1Upgrade'
    tree_name_reEmu = 'l1UpgradeEmuTree/L1UpgradeTree/L1Upgrade'

    file = uproot.open(file_path)
    tree_unpacked = file[tree_name_unpacked]
    tree_reEmu = file[tree_name_reEmu]

    variables = ['tauEt', 'tauEta', 'tauPhi']
    for variable in variables: 
        data_unpacked = ak.flatten(tree_unpacked[variable].array())
        data_reEmu    = ak.flatten(tree_reEmu[variable].array())
        plot_histogram(variable, data_unpacked, data_reEmu, args.tag)

if __name__ == "__main__":
    main()

