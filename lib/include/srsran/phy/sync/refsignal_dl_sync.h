/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_REFSIGNAL_DL_SYNC_H_
#define SRSRAN_REFSIGNAL_DL_SYNC_H_

#include <srsran/phy/ch_estimation/refsignal_dl.h>
#include <srsran/phy/dft/ofdm.h>
#include <srsran/phy/utils/convolution.h>

typedef struct {
  srsran_refsignal_t   refsignal;
  srsran_ofdm_t        ifft;
  cf_t*                ifft_buffer_in;
  cf_t*                ifft_buffer_out;
  cf_t*                sequences[SRSRAN_NOF_SF_X_FRAME];
  cf_t*                correlation;
  srsran_conv_fft_cc_t conv_fft_cc;

  // Results
  bool     found;
  float    rsrp_dBfs;
  float    rssi_dBfs;
  float    rsrq_dB;
  float    cfo_Hz;
  uint32_t peak_index;

  // Figures of the stages of the last run, kept whether or not the cell was
  // found, so a caller can see which stage rejected a cell and by how much.
  // Stage 1: the largest correlation magnitude in the window, the mean RMS of
  // the correlation over the subframes, their ratio (judged against
  // corr_threshold) and the position of the peak, -1 when no block was
  // correlated. Stage 2: the subframes that entered the averages, the RSRP and
  // CFO spreads over them, the mean CFO, the RSRP margin over the sequence of
  // the next subframe and the SSS strength ratio against the half-frame
  // alternative. Stage 3: the count of mild faults and the verdict.
  float    corr_peak;
  float    corr_rms;
  float    corr_ratio;
  int32_t  corr_peak_index;
  uint32_t sf_measured;
  float    rsrp_spread_dB;
  float    cfo_spread_Hz;
  float    cfo_est_Hz;
  float    rsrp_false_margin_dB;
  float    sss_ratio;
  uint32_t false_count;
  bool     false_alarm;

  // Stage 1 threshold on corr_ratio. The library's constant unless set.
  float corr_threshold;
} srsran_refsignal_dl_sync_t;

SRSRAN_API int srsran_refsignal_dl_sync_init(srsran_refsignal_dl_sync_t* q, srsran_cp_t cp);

SRSRAN_API int srsran_refsignal_dl_sync_set_cell(srsran_refsignal_dl_sync_t* q, srsran_cell_t cell);

SRSRAN_API void srsran_refsignal_dl_sync_free(srsran_refsignal_dl_sync_t* q);

SRSRAN_API int srsran_refsignal_dl_sync_run(srsran_refsignal_dl_sync_t* q, cf_t* buffer, uint32_t nsamples);

/* Sets the stage 1 threshold on the correlation peak over the RMS. A value of
 * zero or less restores the library's constant. */
SRSRAN_API void srsran_refsignal_dl_sync_set_correlation_threshold(srsran_refsignal_dl_sync_t* q, float threshold);

SRSRAN_API void srsran_refsignal_dl_sync_measure_sf(srsran_refsignal_dl_sync_t* q,
                                                    cf_t*                       buffer,
                                                    uint32_t                    sf_idx,
                                                    float*                      rsrp,
                                                    float*                      rssi,
                                                    float*                      cfo);

#endif // SRSRAN_REFSIGNAL_DL_SYNC_H_
