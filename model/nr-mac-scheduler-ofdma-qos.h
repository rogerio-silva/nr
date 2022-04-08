/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

// Copyright (c) 2022 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once
#include "nr-mac-scheduler-ofdma-rr.h"

namespace ns3
{

/**
 * \ingroup scheduler
 * \brief Assign frequencies in QoS-based fashion
 *
 * Sort the UE based on the QoS metric (PF + priority) from the minimum QCI
 * among all the QCIs of the LCs activated for a UE.
 *
 * Details in the class NrMacSchedulerUeInfoQos.
 *
 */
class NrMacSchedulerOfdmaQos : public NrMacSchedulerOfdmaRR
{
  public:
    /**
     * \brief GetTypeId
     * \return The TypeId of the class
     */
    static TypeId GetTypeId();
    /**
     * \brief NrMacSchedulerOfdmaQos constructor
     */
    NrMacSchedulerOfdmaQos();

    /**
     * \brief ~NrMacSchedulerOfdmaQos deconstructor
     */
    ~NrMacSchedulerOfdmaQos() override
    {
    }

    /**
     * \brief Set the value of attribute "FairnessIndex"
     * \param v
     */
    void SetFairnessIndex(double v);

    /**
     * \brief Get the value of attribute "FairnessIndex"
     * @return
     */
    double GetFairnessIndex() const;

    /**
     * \brief Set the attribute "LastAvgTPutWeight"
     * \param v the value to save
     */
    void SetTimeWindow(double v);
    /**
     * \brief Get the attribute "LastAvgTPutWeight"
     * \return the value of the attribute
     */
    double GetTimeWindow() const;

  protected:
    // inherit
    /**
     * \brief Create an UE representation of the type NrMacSchedulerUeInfoQos
     * \param params parameters
     * \return NrMacSchedulerUeInfo instance
     */
    std::shared_ptr<NrMacSchedulerUeInfo> CreateUeRepresentation(
            const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const override;

    /**
     * \brief Return the comparison function to sort DL UE according to the scheduler policy
     * \return a pointer to NrMacSchedulerUeInfoQos::CompareUeWeightsDl
     */
    std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                       const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
    GetUeCompareDlFn() const override;

    /**
     * \brief Return the comparison function to sort UL UE according to the scheduler policy
     * \return a pointer to NrMacSchedulerUeInfoQos::CompareUeWeightsUl
     */
    std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                       const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
    GetUeCompareUlFn() const override;

    /**
     * \brief Update the UE representation after a symbol (DL) has been assigned to it
     * \param ue UE to which a symbol has been assigned
     * \param assigned the amount of resources assigned
     * \param totAssigned the total amount of resources assigned in the slot
     *
     * The DL metrics (current Throughput and average Throughput) will be updated
     * by calling the NrMacSchedulerUeInfoQos::UpdateDlQosMetric, which in turn will
     * call NrMacSchedulerUeInfo::UpdateDlMetric in order to get the tbSize
     * based on the resources assigned to the user. This will help the sorting
     * function to sort the UEs for resource allocation.
     */
    void AssignedDlResources(const UePtrAndBufferReq& ue,
                             const FTResources& assigned,
                             const FTResources& totAssigned) const override;

    /**
     * \brief Update DL metrics by calling NrMacSchedulerUeInfoQos::UpdateQosDlMetric
     * \param ue UE to update (ue that didn't get any resources)
     * \param notAssigned the amount of resources not assigned
     * \param totAssigned the total amount of resources assigned in the slot
     *
     * Even if the UE did not get any resource assigned, change its current throughput
     * over the total number of symbols assigned.
     *
     * The DL metrics (current Throughput and average Throughput) will be updated
     * by calling the NrMacSchedulerUeInfoQos::UpdateDlQosMetric, which in turn will
     * call NrMacSchedulerUeInfo::UpdateDlMetric in order to get the tbSize
     * based on the resources assigned to the user. Since no resources have been
     * assigned, the tbSize will be zero. This will help the sorting function to
     * sort the UEs for resource allocation.
     */
    void NotAssignedDlResources(const UePtrAndBufferReq& ue,
                                const FTResources& notAssigned,
                                const FTResources& totalAssigned) const override;

    /**
     * \brief Update the UE representation after a symbol (UL) has been assigned to it
     * \param ue UE to which a symbol has been assigned
     * \param assigned the amount of resources assigned
     * \param totAssigned the total amount of resources assigned in the slot
     *
     * The UL metrics (current Throughput and average Throughput) will be updated
     * by calling the NrMacSchedulerUeInfoQos::UpdateUlQosMetric, which in turn will
     * call NrMacSchedulerUeInfo::UpdateUlMetric in order to get the tbSize
     * based on the resources assigned to the user. This will help the sorting
     * function to sort the UEs for resource allocation.
     */
    void AssignedUlResources(const UePtrAndBufferReq& ue,
                             const FTResources& assigned,
                             const FTResources& totAssigned) const override;

    /**
     * \brief Update UL metrics by calling NrMacSchedulerUeInfoQos::UpdateQosUlMetric
     * \param ue UE to update (ue that didn't get any resources)
     * \param notAssigned the amount of resources not assigned
     * \param totAssigned the total amount of resources assigned in the slot
     *
     * Even if the UE did not get any resource assigned, change its current throughput
     * over the total number of symbols assigned.
     *
     * The UL metrics (current Throughput and average Throughput) will be updated
     * by calling the NrMacSchedulerUeInfoQos::UpdateUlQosMetric, which in turn will
     * call NrMacSchedulerUeInfo::UpdateUlMetric in order to get the tbSize
     * based on the resources assigned to the user. Since no resources have been
     * assigned, the tbSize will be zero. This will help the sorting function to
     * sort the UEs for resource allocation.
     */
    void NotAssignedUlResources(const UePtrAndBufferReq& ue,
                                const FTResources& notAssigned,
                                const FTResources& totalAssigned) const override;

    /**
     * \brief Calculate the potential throughput for the DL based on the available resources
     * \param ue UE to which a rgb has been assigned
     * \param assignableInIteration the minimum amount of resources to be assigned
     *
     * Calculates the the potential throughput by calling
     * NrMacSchedulerUeInfoQos::CalculatePotentialTPutDl.
     */
    void BeforeDlSched(const UePtrAndBufferReq& ue,
                       const FTResources& assignableInIteration) const override;

    /**
     * \brief Calculate the potential throughput for the UL based on the available resources
     * \param ue UE to which a rbg has been assigned
     * \param assignableInIteration the minimum amount of resources to be assigned
     *
     * Calculates the the potential throughput by calling
     * NrMacSchedulerUeInfoQos::CalculatePotentialTPutUl.
     */
    void BeforeUlSched(const UePtrAndBufferReq& ue,
                       const FTResources& assignableInIteration) const override;

  private:
    double m_timeWindow{
        99.0}; //!< Time window to calculate the throughput. Better to make it an attribute.
    double m_alpha{0.0}; //!< PF Fairness index
};

} // namespace ns3
