#!/usr/bin/env python
# encoding: utf-8
"""
test_gmm.py

Simple Test File for GMMs

Copyright (C) 2014 Ircam - Jules Francoise. All Rights Reserved.
author: Jules Francoise <jules.francoise@ircam.fr>
"""

import numpy as np
import matplotlib.pyplot as plt
import mhmm


def test_gmm():
    """ Simple Test Function for Gaussian Mixture Models.
    The data originates from the help patch: "mubu.gmm.maxhelp".
    """
    # Create GMMGroup (group of GMMs running in parallel for recognition)
    multi_gmm = mhmm.GMMGroup()
    # Read trained model from Json file
    multi_gmm.readFile('gmm_test_model.json')
    # read test data (concatenation of the 3 training examples)
    test_data = np.genfromtxt('gmm_test_data1.txt')
    test_data = np.vstack((test_data, np.genfromtxt('gmm_test_data2.txt')))
    test_data = np.vstack((test_data, np.genfromtxt('gmm_test_data3.txt')))
    # Set Size of the likelihood Window (samples)
    multi_gmm.set_likelihoodwindow(20)
    # Initialize performance phase
    multi_gmm.performance_init()
    # Create likelihood array for recognition
    instantaneous_likelihoods = np.zeros((test_data.shape[0], multi_gmm.size()))
    normalized_likelihoods = np.zeros((test_data.shape[0], multi_gmm.size()))
    log_likelihoods = np.zeros((test_data.shape[0], multi_gmm.size()))
    # Performance: Play test data and record the likelihoods of the modes
    for i in range(test_data.shape[0]):
        multi_gmm.performance_update(mhmm.vectorf(test_data[i, :]))
        instantaneous_likelihoods[i, :] = np.array(multi_gmm.results_instant_likelihoods)
        normalized_likelihoods[i, :] = np.array(multi_gmm.results_normalized_likelihoods)
        log_likelihoods[i, :] = np.array(multi_gmm.results_log_likelihoods)
    np.savetxt("gmm_results_normalized_likelihoods.txt", normalized_likelihoods)
    np.savetxt("gmm_results_instantaneous_likelihoods.txt", instantaneous_likelihoods)
    np.savetxt("gmm_results_log_likelihoods.txt", log_likelihoods)
    # Plot the likelihoods over time for the test phase
    plt.figure()
    plt.subplot(311)
    plt.plot(instantaneous_likelihoods)
    plt.title("Instantaneous Likelihood of Each Model Over time")
    plt.xlabel("Time (Samples)")
    plt.ylabel("Likelihood")
    plt.legend(("model 1", "model 2", "model 3"))
    plt.subplot(312)
    plt.plot(normalized_likelihoods)
    plt.title("Normalized Smoothed Likelihood of Each Model Over time")
    plt.xlabel("Time (Samples)")
    plt.ylabel("Normalized Likelihood")
    plt.legend(("model 1", "model 2", "model 3"))
    plt.subplot(313)
    plt.plot(log_likelihoods)
    plt.title("Smoothed Log-Likelihood of Each Model Over time")
    plt.xlabel("Time (Samples)")
    plt.ylabel("Log-Likelihood")
    plt.legend(("model 1", "model 2", "model 3"))
    plt.show()


if __name__ == '__main__':
    test_gmm()