/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-*/
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "WebMWriter.h"
#include "EbmlComposer.h"

namespace mozilla {

WebMWriter::WebMWriter(uint32_t aTrackTypes) : ContainerWriter()
{
  mMetadataRequiredFlag = aTrackTypes;
  mEbmlComposer = new EbmlComposer();
}

nsresult
WebMWriter::WriteEncodedTrack(const EncodedFrameContainer& aData,
                              uint32_t aFlags)
{
  for (uint32_t i = 0 ; i < aData.GetEncodedFrames().Length(); i++) {
    mEbmlComposer->WriteSimpleBlock(aData.GetEncodedFrames().ElementAt(i).get());
  }
  return NS_OK;
}

nsresult
WebMWriter::GetContainerData(nsTArray<nsTArray<uint8_t> >* aOutputBufs,
                             uint32_t aFlags)
{
  mEbmlComposer->ExtractBuffer(aOutputBufs, aFlags);
  if (aFlags & ContainerWriter::FLUSH_NEEDED) {
    mIsWritingComplete = true;
  }
  return NS_OK;
}

nsresult
WebMWriter::SetMetadata(TrackMetadataBase* aMetadata)
{
  MOZ_ASSERT(aMetadata);
  if (aMetadata->GetKind() == TrackMetadataBase::METADATA_VP8) {
    VP8Metadata* meta = static_cast<VP8Metadata*>(aMetadata);
    MOZ_ASSERT(meta, "Cannot find vp8 encoder metadata");
    mEbmlComposer->SetVideoConfig(meta->mWidth, meta->mHeight,
                                  meta->mEncodedFrameRate);
    mMetadataRequiredFlag = mMetadataRequiredFlag & ~ContainerWriter::HAS_VIDEO;
  }

  if (aMetadata->GetKind() == TrackMetadataBase::METADATA_VORBIS) {
    VorbisMetadata* meta = static_cast<VorbisMetadata*>(aMetadata);
    MOZ_ASSERT(meta, "Cannot find vorbis encoder metadata");
    mEbmlComposer->SetAudioConfig(meta->mSamplingFrequency, meta->mChannels, meta->mBitDepth);
    mEbmlComposer->SetAudioCodecPrivateData(meta->mData);
    mMetadataRequiredFlag = mMetadataRequiredFlag & ~ContainerWriter::HAS_AUDIO;
  }

  if (!mMetadataRequiredFlag) {
    mEbmlComposer->GenerateHeader();
  }
  return NS_OK;
}

} // mozilla namespace
