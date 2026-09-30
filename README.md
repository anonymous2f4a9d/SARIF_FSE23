# SARIF_FSE23
This repository contains data and demo for the paper "Software Architecture Recovery with Information Fusion".


#### Data
The *Data* folder is organized as follows:

    ├───Ground Truth Architectures
    │   ├───Collected
    │   └───Labeled
    ├───Metric Tests (RQ1)
    ├───Recovered Architectures (Baseline Techniques)
    │   ├───baseline_metric_results.json
    │   ├───ArchStudio4
    │   ├───bash-4.2
    │   ├───...
    ├───Recovered Architectures (SARIF Default)
    │   ├───ArchStudio4
    │   ├───bash-4.2
    │   ├───...
    ├───Recovered Architectures (SARIF Incomplete)
    │   ├───ArchStudio4
    │   ├───bash-4.2
    │   ├───...
    └───Extensive Experiment
        ├───Labeled Architecture
        ├───Recovered Architectures (Baseline Techniques)
        ├───Recovered Architectures (SARIF 23)
        └───...


- '*Ground Truth Architectures/*' contains ground truth architectures of 8 tested software systems.
- '*Metric Tests (RQ1)/*' contains all metric results for RQ1.
- '*Recovered Architectures (Baseline Techniques)/*' contains the architectures recovered by 7 baseline models. The metrics results of all the baseline techniques are stored in *baseline_metric_results.json*.
- '*Recovered Architectures (SARIF Default)/*' contains the architectures recovered by SARIF with default setups.
- '*Recovered Architectures (SARIF Incomplete)/*' contains the architectures recovered by SARIF with different setups (Ablation). 
- '*Extensive Experiment/*' contains the data related to the Extensive Experiment, including 1) labeled 23 architectures, 2) baseline results for these 23 projects, and 3) SARIF results for all 900 projects.

## Interoperability with ARCADE evaluators

The optional dependency-free exporter converts a flat `cluster_result.json` to
`contain <cluster> <entity>` RSF, preserving full entity paths:

```bash
python tools/export_rsf.py \
  --input 'Data/Recovered Architectures (SARIF Default)/ArchStudio4/cluster_result.json' \
  --output results/archstudio.rsf
python -m unittest discover -s tests -p test_export_rsf.py -v
```

The exporter validates the entire partition before writing. It rejects duplicate
entities/clusters, empty clusters, unsupported nested groups and whitespace in
identifiers, which would make whitespace-delimited RSF ambiguous. It does not
strip project roots or collapse entities to basenames. Align entity universes
explicitly before comparing tools; cluster counts and preprocessing can differ.
The GitHub Action checks the adapter and converts the published Bash/ArchStudio4
architectures. It does not execute the SARIF recovery binary.

## Demo
The *Demo* folder contains our binary demo and a corresponding readme file.
