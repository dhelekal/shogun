/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013 Viktor Gal
 */

#ifndef _WIN32
#include <unistd.h>
#endif
#include <gtest/gtest.h>

#include <shogun/features/streaming/StreamingDenseFeatures.h>
#include <shogun/io/CSVFile.h>
#include <shogun/io/streaming/StreamingAsciiFile.h>
#include "../utils/Utils.h"

using namespace shogun;

TEST(StreamingDenseFeaturesTest, example_reading_from_file)
{
	index_t n=20;
	index_t dim=2;
	char fname[] = "StreamingDenseFeatures_reading.XXXXXX";
	generate_temp_filename(fname);
	auto m_rng = std::unique_ptr<CRandom>(new CRandom());
	SGMatrix<float64_t> data(dim,n);
	for (index_t i=0; i<dim*n; ++i)
		data.matrix[i] = m_rng->std_normal_distrib();

	CDenseFeatures<float64_t>* orig_feats=new CDenseFeatures<float64_t>(data);
	CCSVFile* saved_features = new CCSVFile(fname, 'w');
	orig_feats->save(saved_features);
	saved_features->close();
	SG_UNREF(saved_features);

	CStreamingAsciiFile* input = new CStreamingAsciiFile(fname);
	input->set_delimiter(',');
	CStreamingDenseFeatures<float64_t>* feats
		= new CStreamingDenseFeatures<float64_t>(input, false, 5);

	index_t i = 0;
	feats->start_parser();
	while (feats->get_next_example())
	{
		SGVector<float64_t> example = feats->get_vector();
		SGVector<float64_t> expected = orig_feats->get_feature_vector(i);

		ASSERT_EQ(dim, example.vlen);

		for (index_t j = 0; j < dim; j++)
			EXPECT_NEAR(expected.vector[j], example.vector[j], 1E-5);

		feats->release_example();
		i++;
	}
	feats->end_parser();

	SG_UNREF(orig_feats);
	SG_UNREF(feats);

	std::remove(fname);
}

TEST(StreamingDenseFeaturesTest, example_reading_from_features)
{
	index_t n=20;
	index_t dim=2;
	auto m_rng = std::unique_ptr<CRandom>(new CRandom());
	SGMatrix<float64_t> data(dim,n);
	for (index_t i=0; i<dim*n; ++i)
		data.matrix[i] = m_rng->std_normal_distrib();

	CDenseFeatures<float64_t>* orig_feats=new CDenseFeatures<float64_t>(data);
	CStreamingDenseFeatures<float64_t>* feats = new CStreamingDenseFeatures<float64_t>(orig_feats);

	index_t i = 0;
	feats->start_parser();
	while (feats->get_next_example())
	{
		SGVector<float64_t> example = feats->get_vector();
		SGVector<float64_t> expected = orig_feats->get_feature_vector(i);

		ASSERT_EQ(dim, example.vlen);

		for (index_t j = 0; j < dim; j++)
			EXPECT_DOUBLE_EQ(expected.vector[j], example.vector[j]);

		feats->release_example();
		i++;
	}
	feats->end_parser();

	SG_UNREF(feats);
}

TEST(StreamingDenseFeaturesTest, reset_stream)
{
	index_t n=20;
	index_t dim=2;
	auto m_rng = std::unique_ptr<CRandom>(new CRandom());
	SGMatrix<float64_t> data(dim,n);
	for (index_t i=0; i<dim*n; ++i)
		data.matrix[i] = m_rng->std_normal_distrib();

	CDenseFeatures<float64_t>* orig_feats=new CDenseFeatures<float64_t>(data);
	CStreamingDenseFeatures<float64_t>* feats=new CStreamingDenseFeatures<float64_t>(orig_feats);

	feats->start_parser();

	CDenseFeatures<float64_t>* streamed=dynamic_cast<CDenseFeatures<float64_t>*>(feats->get_streamed_features(n));
	ASSERT_TRUE(streamed!=nullptr);
	ASSERT_TRUE(orig_feats->equals(streamed));
	SG_UNREF(streamed);

	feats->reset_stream();

	streamed=dynamic_cast<CDenseFeatures<float64_t>*>(feats->get_streamed_features(n));
	ASSERT_TRUE(streamed!=nullptr);
	ASSERT_TRUE(orig_feats->equals(streamed));
	SG_UNREF(streamed);

	feats->end_parser();
	SG_UNREF(feats);
}
