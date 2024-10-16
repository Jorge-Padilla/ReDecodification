/*
 * Copyright (c) 2010, 2016, 2021 ARM Limited
 * Copyright (c) 2013 Advanced Micro Devices, Inc.
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2004-2006 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cpu/o3/affinity_table.hh"

#include <algorithm>
#include <array>
#include <deque>
#include <list>
#include <string>

#include "cpu/op_class.hh"

namespace gem5
{

namespace o3
{

AffinityTable::AffinityTable()
{
    //GPR -> SCALAR
    __affinityTable[IntAluOp] = SimdAluOp;
    __affinityTable[IntMultOp] = SimdMultOp;
    __affinityTable[IntDivOp] = SimdDivOp;
    __affinityTable[FloatAddOp] = SimdFloatAddOp;
    __affinityTable[FloatCmpOp] = SimdFloatCmpOp;
    __affinityTable[FloatCvtOp] = SimdFloatCvtOp;
    __affinityTable[FloatMultOp] = SimdFloatMultOp;
    __affinityTable[FloatMultAccOp] = SimdFloatMultAccOp;
    __affinityTable[FloatDivOp] = SimdFloatDivOp;
    __affinityTable[FloatMiscOp] = SimdFloatMiscOp;
    __affinityTable[FloatSqrtOp] = SimdFloatSqrtOp;
    // SCALAR -> Vector
    __affinityTable[SimdAddOp] = SimdShadowAddOp;
    __affinityTable[SimdAddAccOp] = SimdShadowAddAccOp;
    __affinityTable[SimdAluOp] = SimdShadowAluOp;
    __affinityTable[SimdCmpOp] = SimdShadowCmpOp;
    __affinityTable[SimdCvtOp] = SimdShadowCvtOp;
    __affinityTable[SimdMiscOp] = SimdShadowMiscOp;
    __affinityTable[SimdMultOp] = SimdShadowMultOp;
    __affinityTable[SimdMultAccOp] = SimdShadowMultAccOp;
    __affinityTable[SimdMatMultAccOp] = SimdShadowMatMultAccOp;
    __affinityTable[SimdShiftOp] = SimdShadowShiftOp;
    __affinityTable[SimdShiftAccOp] = SimdShadowShiftAccOp;
    __affinityTable[SimdDivOp] = SimdShadowDivOp;
    __affinityTable[SimdSqrtOp] = SimdShadowSqrtOp;
    __affinityTable[SimdReduceAddOp] = SimdShadowReduceAddOp;
    __affinityTable[SimdReduceAluOp] = SimdShadowReduceAluOp;
    __affinityTable[SimdReduceCmpOp] = SimdShadowReduceCmpOp;
    __affinityTable[SimdFloatAddOp] = SimdShadowFloatAddOp;
    __affinityTable[SimdFloatAluOp] = SimdShadowFloatAluOp;
    __affinityTable[SimdFloatCmpOp] = SimdShadowFloatCmpOp;
    __affinityTable[SimdFloatCvtOp] = SimdShadowFloatCvtOp;
    __affinityTable[SimdFloatDivOp] = SimdShadowFloatDivOp;
    __affinityTable[SimdFloatMiscOp] = SimdShadowFloatMiscOp;
    __affinityTable[SimdFloatMultOp] = SimdShadowFloatMultOp;
    __affinityTable[SimdFloatMultAccOp] = SimdShadowFloatMultAccOp;
    __affinityTable[SimdFloatMatMultAccOp] = SimdShadowFloatMatMultAccOp;
    __affinityTable[SimdFloatSqrtOp] = SimdShadowFloatSqrtOp;
    __affinityTable[SimdFloatReduceCmpOp] = SimdShadowFloatReduceCmpOp;
    __affinityTable[SimdFloatReduceAddOp] = SimdShadowFloatReduceAddOp;
    __affinityTable[SimdAesOp] = SimdAesOp;
    __affinityTable[SimdAesMixOp] = SimdAesMixOp;
    __affinityTable[SimdSha1HashOp] = SimdSha1HashOp;
    __affinityTable[SimdSha1Hash2Op] = SimdSha1Hash2Op;
    __affinityTable[SimdSha256HashOp] = SimdSha256HashOp;
    __affinityTable[SimdSha256Hash2Op] = SimdSha256Hash2Op;
    __affinityTable[SimdShaSigma2Op] = SimdShaSigma2Op;
    __affinityTable[SimdShaSigma3Op] = SimdShaSigma3Op;
    __affinityTable[SimdPredAluOp] = SimdPredAluOp;
}

} // namespace o3
} // namespace gem5
