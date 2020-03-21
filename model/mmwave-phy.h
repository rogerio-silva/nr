/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
*   Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
*   Copyright (c) 2015 NYU WIRELESS, Tandon School of Engineering, New York University
*   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation;
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/

#ifndef SRC_MMWAVE_MODEL_MMWAVE_PHY_H_
#define SRC_MMWAVE_MODEL_MMWAVE_PHY_H_

#include "mmwave-phy-sap.h"
#include <ns3/spectrum-value.h>

namespace ns3 {

class MmWaveNetDevice;
class MmWaveControlMessage;
class MmWaveSpectrumPhy;
class AntennaArrayBasicModel;
class BeamManager;
class ThreeGppAntennaArrayModel;

class MmWavePhy : public Object
{
public:
  MmWavePhy ();

  virtual ~MmWavePhy () override;

  static TypeId GetTypeId (void);

  // Called by SAP
  /**
   * \brief Enqueue a ctrl message, keeping in consideration L1L2CtrlDelay
   * \param m the message to enqueue
   */
  void EnqueueCtrlMessage (const Ptr<MmWaveControlMessage> &m);

  void SetMacPdu (Ptr<Packet> pb);
  void SendRachPreamble (uint32_t PreambleId, uint32_t Rnti);

  /**
   * \brief Store the slot allocation info
   * \param slotAllocInfo the allocation to store
   *
   * This method expect that the sfn of the allocation will match the sfn
   * when the allocation will be retrieved.
   */
  void PushBackSlotAllocInfo (const SlotAllocInfo &slotAllocInfo);

  /**
   * \brief Notify PHY about the successful RRC connection
   * establishment.
   */
  void NotifyConnectionSuccessful ();

  /**
   * \brief Configures TB decode latency
   */
  virtual void SetTbDecodeLatency (Time us);

  /**
   * \brief Returns TB decode latency
   */
  virtual Time GetTbDecodeLatency (void) const;

  virtual BeamId GetBeamId (uint16_t rnti) const = 0;

  void InstallAntenna (const Ptr<ThreeGppAntennaArrayModel> &antenna);

  // Note: Returning a BeamManger, it means that someone outside this class
  // can change the beamforming vector, BUT the phy will not learn it.
  Ptr<BeamManager> GetBeamManager () const;

  Ptr<const SpectrumModel> GetSpectrumModel () const;

  uint32_t GetSymbolsPerSlot () const;

  Time GetSlotPeriod () const;

  /**
   * \brief GetNumScsPerRb
   *
   * It is a static function as the value is fixed, and it is needed in NrAmc.
   * Making this value changeable means creating an interface between PHY
   * and AMC.
   *
   * \return 12, fixed value
   */
  static uint32_t GetNumScsPerRb ();

  /**
   * \brief Get SymbolPeriod
   * \return the symbol period; the value changes every time the user changes
   * the numerology
   */
  Time GetSymbolPeriod () const;

  // Attributes
  /**
   * \return The antena array that is being used by this PHY
   */
  Ptr<const ThreeGppAntennaArrayModel> GetAntennaArray () const;

  void SetNoiseFigure (double d);
  double GetNoiseFigure () const;

  // Installation / Helpers
  MmWavePhySapProvider* GetPhySapProvider ();

  void SetDevice (Ptr<MmWaveNetDevice> d);

  /**
   * \brief Install the PHY over a particular central frequency
   * \param f the frequency in Hz
   * The helper will call this function, making sure it is equal to the value
   * of the channel.
   */
  void InstallCentralFrequency (double f);

  /**
   * \brief Set GNB or UE numerology
   * \param numerology numerology
   *
   * For the GNB, this is an attribute that can be changed at any time; for the
   * UE, it is set by the helper at the attachment, and then is not changed anymore.
   */
  void SetNumerology (uint16_t numerology);

  uint16_t GetNumerology () const;

  void SetSymbolsPerSlot (uint16_t symbolsPerSlot);

  /**
   * \brief Retrieve the SpectrumPhy pointer
   *
   * As this function is used mainly to get traced values out of Spectrum,
   * it should be removed and the traces connected (and redirected) here.
   * \return A pointer to the SpectrumPhy of this UE
   */
  Ptr<MmWaveSpectrumPhy> GetSpectrumPhy () const;

  /**
   * \brief Set the SpectrumPhy associated with this PHY
   * \param spectrumPhy the spectrumPhy
   */
  void InstallSpectrumPhy (const Ptr<MmWaveSpectrumPhy> &spectrumPhy);

  virtual void ScheduleStartEventLoop (uint32_t nodeId, uint16_t frame, uint8_t subframe, uint16_t slot) = 0;

  /**
   * \brief Set the bwp id. Called by helper.
   */
  void SetBwpId (uint16_t bwpId);

  /**
   * \return the BWP ID
   */
  uint16_t GetBwpId () const;

  /**
   * \return the cell ID
   */
  uint16_t GetCellId () const;

  /**
   * \brief Get the number of Resource block configured
   *
   * It changes with the numerology and the channel bandwidth
   */
  uint32_t GetRbNum () const;

  /**
   * \return the latency (in n. of slots) between L1 and L2 layers. Default to 2.
   *
   * Before it was an attribute; as we are unsure if it works for values different
   * from 2, we decided to make it static until the need to have it different
   * from 2 arises.
   */
  uint32_t GetL1L2CtrlLatency () const;

  // SAP
  /**
   * \brief Set the cell ID
   * \param cellId the cell id.
   *
   * Called by lte-enb-cphy-sap only.
   */
  void DoSetCellId (uint16_t cellId);

  /**
   * \brief Take the control messages, and put it in a list that will be sent at the first occasion
   * \param msg Message to "encode" and transmit
   */
  void EncodeCtrlMsg (const Ptr<MmWaveControlMessage> &msg);

protected:
  /**
   * \brief Update the number of RB. Usually called after bandwidth changes
   */
  void UpdateRbNum ();

  static bool IsTdd (const std::vector<LteNrTddSlotType> &pattern);

  /**
   * \brief Transform a MAC-made vector of RBG to a PHY-ready vector of SINR indices
   * \param rbgBitmask Bitmask which indicates with 1 the RBG in which there is a transmission,
   * with 0 a RBG in which there is not a transmission
   * \return a vector of indices.
   *
   * Example (4 RB per RBG, 4 total RBG assignable):
   * rbgBitmask = <0,1,1,0>
   * output = <4,5,6,7,8,9,10,11>
   *
   * (the rbgBitmask expressed as rbBitmask would be:
   * <0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0> , and therefore the places in which there
   * is a 1 are from the 4th to the 11th, and that is reflected in the output)
   */
  std::vector<int> FromRBGBitmaskToRBAssignment (const std::vector<uint8_t> rbgBitmask) const;

  /**
   * \brief Protected function that is used to get the number of resource
   * blocks per resource block group.
   * \return Returns the number of RBs per RBG
   */
  virtual uint32_t GetNumRbPerRbg () const = 0;

  /**
   * \return the channel bandwidth in Hz
   */
  virtual uint32_t GetChannelBandwidth () const = 0;

  Ptr<PacketBurst> GetPacketBurst (SfnSf, uint8_t sym);

  /**
   * \brief Create Noise Power Spectral density
   * \return A SpectrumValue array with fixed size, in which a value is
   * update to a particular value of the noise
   */
  Ptr<SpectrumValue> GetNoisePowerSpectralDensity () const;

  /**
   * Create Tx Power Spectral Density
   * \param rbIndexVector vector of the index of the RB (in SpectrumValue array)
   * in which there is a transmission
   * \return A SpectrumValue array with fixed size, in which each value
   * is updated to a particular value if the correspond RB index was inside the rbIndexVector,
   * or is left untouched otherwise.
   * \see MmWaveSpectrumValueHelper::CreateTxPowerSpectralDensity
   */
  Ptr<SpectrumValue> GetTxPowerSpectralDensity (const std::vector<int> &rbIndexVector) const;

  /**
   * \brief Retrieve the frequency (in Hz) of this PHY's channel
   * \return the frequency of the channel in Hz
   *
   * The function will assert if it is called without having set a frequency first.
   */
  double GetCentralFrequency () const;

  /**
   * \brief Store the slot allocation info at the front
   * \param slotAllocInfo the allocation to store
   *
   * Increase the sfn of all allocations to be chronologically "in order".
   */
  void PushFrontSlotAllocInfo (const SfnSf &newSfnSf, const SlotAllocInfo &slotAllocInfo);

  /**
   * \brief Check if the SlotAllocationInfo for that slot exists
   * \param sfnsf slot to check
   * \return true if the allocation exists
   */
  bool SlotAllocInfoExists (const SfnSf &sfnsf) const;

  /**
   * \brief Get the head for the slot allocation info, and delete it from the
   * internal list
   * \return the Slot allocation info head
   */
  SlotAllocInfo RetrieveSlotAllocInfo ();

  /**
   * \brief Get the SlotAllocationInfo for the specified slot, and delete it
   * from the internal list
   *
   * \param sfnsf slot specified
   * \return the SlotAllocationInfo
   */
  SlotAllocInfo RetrieveSlotAllocInfo (const SfnSf &sfnsf);

  /**
   * \brief Peek the SlotAllocInfo at the SfnSf specified
   * \param sfnsf (existing) SfnSf to look for
   * \return a reference to the SlotAllocInfo
   *
   * The method will assert if sfnsf does not exits (please check with SlotExists())
   */
  SlotAllocInfo & PeekSlotAllocInfo (const SfnSf & sfnsf);

  /**
   * \brief Retrieve the size of the SlotAllocInfo list
   * \return the allocation list size
   */
  size_t SlotAllocInfoSize () const;

  /**
   * \brief Check if there are no control messages queued for this slot
   * \return true if there are no control messages queued for this slot
   */
  bool IsCtrlMsgListEmpty () const;

  /**
   * \brief Enqueue a CTRL message without considering L1L2CtrlLatency
   * \param msg The message to enqueue
   */
  void EnqueueCtrlMsgNow (const Ptr<MmWaveControlMessage> &msg);

  /**
   * @brief Enqueue a CTRL message without considering L1L2CtrlLatency
   * @param listOfMsgs The list of messages to enqueue
   */
  void EnqueueCtrlMsgNow (const std::list<Ptr<MmWaveControlMessage> > &listOfMsgs);

  /**
   * \brief Initialize the message list
   */
  void InitializeMessageList ();
  /**
   * \brief Extract and return the message list that is at the beginning of the queue
   * \return a list of control messages that are meant to be sent in the current slot
   */
  virtual std::list<Ptr<MmWaveControlMessage> > PopCurrentSlotCtrlMsgs (void);

protected:
  Ptr<MmWaveNetDevice> m_netDevice;
  Ptr<MmWaveSpectrumPhy> m_spectrumPhy;

  double m_txPower {0.0};
  double m_noiseFigure {0.0};

  std::unordered_map<uint64_t, Ptr<PacketBurst> > m_packetBurstMap;

  SlotAllocInfo m_currSlotAllocInfo;

  MmWavePhySapProvider* m_phySapProvider;

  uint32_t m_raPreambleId {0};
  bool m_isConnected {false}; ///< set when UE RRC is in CONNECTED_NORMALLY state
  Ptr<BeamManager> m_beamManager; //!< TODO

  std::list <Ptr<MmWaveControlMessage>> m_ctrlMsgs; //!< CTRL messages to be sent

  std::vector<LteNrTddSlotType> m_tddPattern = { F, F, F, F, F, F, F, F, F, F}; //!< Pattern

private:
  std::list<SlotAllocInfo> m_slotAllocInfo; //!< slot allocation info list
  std::vector<std::list<Ptr<MmWaveControlMessage>>> m_controlMessageQueue; //!< CTRL message queue

  Time m_tbDecodeLatencyUs {MicroSeconds(100)}; //!< transport block decode latency
  double m_centralFrequency {-1.0}; //!< Channel central frequency -- set by the helper

  uint16_t m_bwpId {UINT16_MAX}; //!< Bwp ID -- in the GNB, it is set by RRC, in the UE, by the helper when attaching to a gnb
  uint16_t m_numerology {0};         //!< NR numerology: defines the subcarrier spacing, RB width, slot length, and number of slots per subframe
  uint16_t m_symbolsPerSlot {14};    //!< number of OFDM symbols per slot (in 3GPP NR: 12 for normal CP, 14 for extended CP)

  // CHECK!
  uint16_t m_slotsPerSubframe {1};
  Time m_slotPeriod {MilliSeconds (1)};            //!< NR slot length
  Time m_symbolPeriod {MilliSeconds (1) / 14};          //!< OFDM symbol length
  uint32_t m_subcarrierSpacing {15000};   //!< subcarrier spacing (it is determined by the numerology)
  uint32_t m_rbNum {0};             //!< number of resource blocks within the channel bandwidth

  uint16_t m_cellId {0}; //!< Cell ID which identify this BWP.
};

}


#endif /* SRC_MMWAVE_MODEL_MMWAVE_PHY_H_ */
