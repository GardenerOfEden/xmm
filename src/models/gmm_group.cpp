//
// concurrent_gmm.cpp
//
// Class for Multiple Gaussian Mixture Models running in Parallel
//
// Copyright (C) 2014 Ircam - Jules Francoise. All Rights Reserved.
// author: Jules Francoise <jules.francoise@ircam.fr>
//

#include "gmm_group.h"

#pragma mark -
#pragma mark Constructor
GMMGroup::GMMGroup(rtml_flags flags,
                             TrainingSet *_globalTrainingSet)
: ModelGroup< GMM >(flags, _globalTrainingSet)
{
    bimodal_ = (flags & BIMODAL);
}

#pragma mark -
#pragma mark Get & Set
int GMMGroup::get_nbMixtureComponents() const
{
    return this->referenceModel_.get_nbMixtureComponents();
}

void GMMGroup::set_nbMixtureComponents(int nbMixtureComponents_)
{
    this->referenceModel_.set_nbMixtureComponents(nbMixtureComponents_);
    for (model_iterator it=this->models.begin(); it != this->models.end(); ++it) {
        it->second.set_nbMixtureComponents(nbMixtureComponents_);
    }
}

float GMMGroup::get_covarianceOffset() const
{
    return this->referenceModel_.get_covarianceOffset();
}

void GMMGroup::set_covarianceOffset(float covarianceOffset_)
{
    this->referenceModel_.set_covarianceOffset(covarianceOffset_);
    for (model_iterator it=this->models.begin(); it != this->models.end(); ++it) {
        it->second.set_covarianceOffset(covarianceOffset_);
    }
}

int GMMGroup::get_EM_minSteps() const
{
    return this->referenceModel_.get_EM_minSteps();
}

int GMMGroup::get_EM_maxSteps() const
{
    return this->referenceModel_.get_EM_maxSteps();
}

double GMMGroup::get_EM_percentChange() const
{
    return this->referenceModel_.get_EM_percentChange();
}

void GMMGroup::set_EM_minSteps(int steps)
{
    this->referenceModel_.set_EM_minSteps(steps);
    for (model_iterator it=this->models.begin(); it != this->models.end(); ++it) {
        it->second.set_EM_minSteps(steps);
    }
}

void GMMGroup::set_EM_maxSteps(int steps)
{
    this->referenceModel_.set_EM_maxSteps(steps);
    for (model_iterator it=this->models.begin(); it != this->models.end(); ++it) {
        it->second.set_EM_maxSteps(steps);
    }
}

void GMMGroup::set_EM_percentChange(double logLikPercentChg_)
{
    this->referenceModel_.set_EM_percentChange(logLikPercentChg_);
    for (model_iterator it=this->models.begin(); it != this->models.end(); ++it) {
        it->second.set_EM_percentChange(logLikPercentChg_);
    }
}

unsigned int GMMGroup::get_likelihoodBufferSize() const
{
    return this->referenceModel_.get_likelihoodBufferSize();
}

void GMMGroup::set_likelihoodBufferSize(unsigned int likelihoodBufferSize_)
{
    this->referenceModel_.set_likelihoodBufferSize(likelihoodBufferSize_);
    for (model_iterator it=this->models.begin(); it != this->models.end(); ++it) {
        it->second.set_likelihoodBufferSize(likelihoodBufferSize_);
    }
}

#pragma mark -
#pragma mark Performance
void GMMGroup::performance_update(vector<float> const& observation)
{
    double norm_const(0.0);
    int i(0);
    model_iterator likeliestModel;
    double currentMaxLikelihood(-100000.);
    for (model_iterator it = this->models.begin(); it != this->models.end(); ++it) {
        results_instant_likelihoods[i] = it->second.performance_update(observation);
        if (results_instant_likelihoods[i] > currentMaxLikelihood) {
            currentMaxLikelihood = results_instant_likelihoods[i];
            likeliestModel = it;
        }
        norm_const += results_instant_likelihoods[i++];
    }
    
    for (unsigned int i=0; i<this->models.size(); i++)
        results_instant_likelihoods[i] /= norm_const;
    
    if (bimodal_) {
        unsigned int dimension = this->referenceModel_.get_dimension();
        unsigned int dimension_input = this->referenceModel_.get_dimension_input();
        unsigned int dimension_output = dimension - dimension_input;
        
        if (this->performanceMode_ == this->LIKELIEST) {
            copy(likeliestModel->second.results_predicted_output.begin(),
                 likeliestModel->second.results_predicted_output.end(),
                 results_predicted_output.begin());
        } else {
            results_predicted_output.assign(dimension_output, 0.0);
            
            int i(0);
            for (model_iterator it=this->models.begin(); it != this->models.end(); ++it) {
                for (int d=0; d<dimension_output; d++) {
                    results_predicted_output[d] += results_predicted_output[i] * it->second.results_predicted_output[d];
                }
                i++;
            }
        }
    }
}

#pragma mark -
#pragma mark File IO
/** @name File IO */
/**
 * @brief Write to JSON Node
 * @return JSON Node containing training set information and data
 */
JSONNode GMMGroup::to_json() const
{
    JSONNode json_ccmodels(JSON_NODE);
    json_ccmodels.set_name("GMMGroup");
    json_ccmodels.push_back(JSONNode("bimodal", bimodal_));
    json_ccmodels.push_back(JSONNode("dimension", get_dimension()));
    if (bimodal_)
        json_ccmodels.push_back(JSONNode("dimension_input", get_dimension_input()));
    json_ccmodels.push_back(JSONNode("size", models.size()));
    json_ccmodels.push_back(JSONNode("performancemode", int(performanceMode_)));
    json_ccmodels.push_back(JSONNode("nbmixturecomponents", get_nbMixtureComponents()));
    json_ccmodels.push_back(JSONNode("covarianceoffset", get_covarianceOffset()));
    
    // Add Models
    JSONNode json_models(JSON_ARRAY);
    for (const_model_iterator it = models.begin(); it != models.end(); ++it)
    {
        JSONNode json_model(JSON_NODE);
        json_model.push_back(it->first.to_json());
        json_model.push_back(it->second.to_json());
        json_models.push_back(json_model);
    }
    json_models.set_name("models");
    json_ccmodels.push_back(json_models);
    
    return json_ccmodels;
}

/**
 * @brief Read from JSON Node
 * @param root JSON Node containing training set information and data
 * @throws JSONException if the JSON Node has a wrong format
 */
void GMMGroup::from_json(JSONNode root)
{
    try {
        assert(root.type() == JSON_NODE);
        JSONNode::const_iterator root_it = root.begin();
        
        // Get Number of modalities
        assert(root_it != root.end());
        assert(root_it->name() == "bimodal");
        assert(root_it->type() == JSON_BOOL);
        if(bimodal_ != root_it->as_bool()) {
            if (bimodal_)
                throw JSONException("Trying to read an unimodal model in a bimodal model.", root.name());
            else
                throw JSONException("Trying to read a bimodal model in an unimodal model.", root.name());
        }
        ++root_it;
        
        // Get Dimension
        assert(root_it != root.end());
        assert(root_it->name() == "dimension");
        assert(root_it->type() == JSON_NUMBER);
        this->referenceModel_.dimension_ = root_it->as_int();
        ++root_it;
        
        // Get Input Dimension if bimodal
        if (bimodal_){
            assert(root_it != root.end());
            assert(root_it->name() == "dimension_input");
            assert(root_it->type() == JSON_NUMBER);
            this->referenceModel_.dimension_input_ = root_it->as_int();
            ++root_it;
        }
        
        // Get Size: Number of Models
        assert(root_it != root.end());
        assert(root_it->name() == "size");
        assert(root_it->type() == JSON_NUMBER);
        int numModels = root_it->as_int();
        ++root_it;
        
        // Get Play Mode
        assert(root_it != root.end());
        assert(root_it->name() == "performancemode");
        assert(root_it->type() == JSON_NUMBER);
        performanceMode_ = (root_it->as_int() > 0) ? MIXTURE : LIKELIEST;
        ++root_it;
        
        // Get Mixture Components
        assert(root_it != root.end());
        assert(root_it->name() == "nbMixturecomponents");
        assert(root_it->type() == JSON_NUMBER);
        set_nbMixtureComponents(root_it->as_int());
        ++root_it;
        
        // Get Covariance Offset
        assert(root_it != root.end());
        assert(root_it->name() == "covarianceoffset");
        assert(root_it->type() == JSON_NUMBER);
        set_covarianceOffset(root_it->as_float());
        ++root_it;
        
        // Get Models
        models.clear();
        assert(root_it != root.end());
        assert(root_it->name() == "models");
        assert(root_it->type() == JSON_ARRAY);
        for (int i=0 ; i<numModels ; i++)
        {
            // Get Label
            JSONNode::const_iterator array_it = (*root_it)[i].begin();
            assert(array_it != root_it->end());
            assert(array_it->name() == "label");
            assert(array_it->type() == JSON_NODE);
            Label l;
            l.from_json(*array_it);
            ++array_it;
            
            // Get Phrase Content
            assert(array_it != root_it->end());
            assert(array_it->type() == JSON_NODE);
            models[l] = this->referenceModel_;
            models[l].trainingSet = NULL;
            models[l].from_json(*array_it);
        }
        
        assert(numModels == models.size());
        
    } catch (JSONException &e) {
        throw JSONException(e);
    } catch (exception &e) {
        throw JSONException(e, root.name());
    }
}
