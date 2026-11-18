import matplotlib.pyplot as plt
import h5py
import numpy as np

with h5py.File("../outputs/leakas.h5") as file:
  runtime = file["runtime"][()][0]
  mesh_z = file["meshes/Z"]
  NUM_Z_BINS = mesh_z["num_bins"][()][0]
  Z_MIN = mesh_z["min"][()][0]
  Z_MAX = mesh_z["max"][()][0]
  DZ = (Z_MAX - Z_MIN )/ NUM_Z_BINS
  mesh_mu = file["meshes/Mu"]
  NUM_MU_BINS = mesh_mu["num_bins"][()][0]
  MU_MIN = mesh_mu["min"][()][0]
  MU_MAX = mesh_mu["max"][()][0]
  DMU = (MU_MAX - MU_MIN) / NUM_MU_BINS

  psi = file["tally/mean"][()].reshape(NUM_Z_BINS, NUM_MU_BINS)

phi = psi.sum(axis=1) * DMU 

z_centers = np.linspace(Z_MIN + DZ / 2, Z_MAX - DZ/2, NUM_Z_BINS)

plt.plot(z_centers, phi)
plt.show()