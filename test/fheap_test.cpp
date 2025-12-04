#include <algorithm>
#include <gtest/gtest.h>
#include <random>
#include <stdexcept>
#include <vector>

#include "fheap.hpp"

namespace {

void
AssertHeapMinIsCorrect( fhp::FHeap& heap, const std::vector<int>& inserted )
{
    if ( inserted.empty() )
    {
        return;
    }
    int reported_min = heap.GetMin();
    int true_min     = *std::min_element( inserted.begin(), inserted.end() );
    EXPECT_EQ( reported_min, true_min );
}

void
AssertHeapContainsExactly( fhp::FHeap heap, std::vector<int> expected )
{
    std::sort( expected.begin(), expected.end() );
    std::vector<int> extracted;
    while ( heap.GetSize() > 0 )
    {
        extracted.push_back( heap.ExtractMin() );
    }
    std::sort( extracted.begin(), extracted.end() );
    EXPECT_EQ( extracted, expected );
}

} // namespace

TEST( FHeapTest, EmptyHeap )
{
    fhp::FHeap heap;
    EXPECT_EQ( heap.GetSize(), 0U );
    EXPECT_THROW( heap.GetMin(), std::runtime_error );
    EXPECT_THROW( heap.ExtractMin(), std::runtime_error );
}

TEST( FHeapTest, SingleElement )
{
    fhp::FHeap heap;
    auto       h = heap.Insert( 42 );
    EXPECT_EQ( heap.GetMin(), 42 );
    EXPECT_EQ( heap.GetSize(), 1U );
    EXPECT_EQ( heap.ExtractMin(), 42 );
    EXPECT_EQ( heap.GetSize(), 0U );
    EXPECT_THROW( heap.GetMin(), std::runtime_error );
}

TEST( FHeapTest, InsertAndExtractAll )
{
    fhp::FHeap       heap;
    std::vector<int> data = { 100, 50, 200, 25, 75 };
    for ( int x : data )
    {
        heap.Insert( x );
    }
    std::sort( data.begin(), data.end() );
    for ( int expected : data )
    {
        ASSERT_GT( heap.GetSize(), 0U );
        EXPECT_EQ( heap.GetMin(), expected );
        heap.ExtractMin();
    }
    EXPECT_EQ( heap.GetSize(), 0U );
}

TEST( FHeapTest, DecreaseKeyToMinimum )
{
    fhp::FHeap heap;
    auto       h100 = heap.Insert( 100 );
    heap.Insert( 200 );
    heap.Insert( 300 );
    EXPECT_EQ( heap.GetMin(), 100 );

    heap.DecreaseKey( h100, 50 );
    EXPECT_EQ( heap.GetMin(), 50 );

    heap.DecreaseKey( h100, 10 );
    EXPECT_EQ( heap.GetMin(), 10 );
}

TEST( FHeapTest, DecreaseKeyInvalid )
{
    fhp::FHeap heap;
    auto       h = heap.Insert( 50 );
    EXPECT_THROW( heap.DecreaseKey( h, 60 ), std::invalid_argument );
}

TEST( FHeapTest, EraseElement )
{
    fhp::FHeap heap;
    auto       h1 = heap.Insert( 100 );
    auto       h2 = heap.Insert( 200 );
    auto       h3 = heap.Insert( 50 );

    heap.Erase( h2 );
    AssertHeapMinIsCorrect( heap, { 100, 50 } );
    AssertHeapContainsExactly( heap, { 50, 100 } );
}

TEST( FHeapTest, EraseMinimum )
{
    fhp::FHeap heap;
    auto       h1 = heap.Insert( 10 );
    heap.Insert( 20 );
    heap.Insert( 30 );
    heap.Erase( h1 );
    EXPECT_EQ( heap.GetMin(), 20 );
}

TEST( FHeapTest, MergeWithEmpty )
{
    fhp::FHeap h1;
    fhp::FHeap h2;
    h1.Insert( 10 );
    h1.Merge( h2 );
    EXPECT_EQ( h1.GetMin(), 10 );
    EXPECT_EQ( h1.GetSize(), 1U );
}

TEST( FHeapTest, MergeTwoNonEmpty )
{
    fhp::FHeap h1;
    fhp::FHeap h2;
    h1.Insert( 100 );
    h1.Insert( 200 );
    h2.Insert( 50 );
    h2.Insert( 300 );
    h1.Merge( h2 );
    EXPECT_EQ( h1.GetMin(), 50 );
    EXPECT_EQ( h1.GetSize(), 4U );
    AssertHeapContainsExactly( h1, { 50, 100, 200, 300 } );
}

TEST( FHeapTest, ExtractFromEmpty )
{
    fhp::FHeap heap;
    EXPECT_THROW( heap.ExtractMin(), std::runtime_error );
}

TEST( FHeapTest, GetMinFromEmpty )
{
    fhp::FHeap heap;
    EXPECT_THROW( heap.GetMin(), std::runtime_error );
}

TEST( FHeapTest, InvalidHandleOperations )
{
    fhp::FHeap         heap;
    fhp::FHeap::Handle invalid_handle;
    EXPECT_THROW( heap.DecreaseKey( invalid_handle, 10 ), std::invalid_argument );
    EXPECT_THROW( heap.Erase( invalid_handle ), std::invalid_argument );
}

TEST( FHeapTest, ManyDuplicates )
{
    fhp::FHeap heap;
    const int  kNumDuplicates = 100;
    for ( int i = 0; i < kNumDuplicates; ++i )
    {
        heap.Insert( 42 );
    }
    EXPECT_EQ( heap.GetMin(), 42 );
    for ( int i = 0; i < kNumDuplicates; ++i )
    {
        EXPECT_EQ( heap.ExtractMin(), 42 );
    }
    EXPECT_EQ( heap.GetSize(), 0U );
}

TEST( FHeapTest, RandomizedOperations )
{
    std::mt19937                       rng( 12345 );
    std::uniform_int_distribution<int> key_dist( 1, 10000 );
    std::uniform_int_distribution<int> op_dist( 0, 1 );

    fhp::FHeap       heap;
    std::vector<int> reference;

    const int kNumOps = 2000;
    for ( int step = 0; step < kNumOps; ++step )
    {
        if ( reference.empty() || op_dist( rng ) == 0 )
        {
            int key = key_dist( rng );
            heap.Insert( key );
            reference.push_back( key );
        } else
        {
            int expected = *std::min_element( reference.begin(), reference.end() );
            int actual   = heap.ExtractMin();
            EXPECT_EQ( actual, expected );
            auto it = std::find( reference.begin(), reference.end(), expected );
            reference.erase( it );
        }

        if ( !reference.empty() )
        {
            int heap_min = heap.GetMin();
            int true_min = *std::min_element( reference.begin(), reference.end() );
            EXPECT_EQ( heap_min, true_min );
        }
    }
}

TEST( FHeapTest, LargeInsertExtract )
{
    fhp::FHeap       heap;
    const size_t     kSize = 10000;
    std::vector<int> data( kSize );
    for ( size_t i = 0; i < kSize; ++i )
    {
        data[i] = static_cast<int>( i + 1 );
    }
    std::shuffle( data.begin(), data.end(), std::mt19937( 42 ) );

    for ( int x : data )
    {
        heap.Insert( x );
    }
    EXPECT_EQ( heap.GetSize(), kSize );

    for ( size_t i = 1; i <= kSize; ++i )
    {
        ASSERT_EQ( heap.GetMin(), static_cast<int>( i ) );
        heap.ExtractMin();
    }
    EXPECT_EQ( heap.GetSize(), 0U );
}

int
main( int argc, char** argv )
{
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
