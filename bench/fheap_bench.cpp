#include <benchmark/benchmark.h>
#include <boost/heap/binomial_heap.hpp>
#include <boost/heap/d_ary_heap.hpp>
#include <boost/heap/fibonacci_heap.hpp>
#include <random>
#include <set>
#include <vector>

#include "fheap.hpp"

std::vector<int>
generateRandomData( size_t n, uint64_t seed = 42 )
{
    std::mt19937                       rng( seed );
    std::uniform_int_distribution<int> dist( 1, 10000000 );
    std::vector<int>                   data;
    data.reserve( n );

    std::set<int> seen;
    while ( data.size() < n )
    {
        int val = dist( rng );
        if ( seen.insert( val ).second )
        {
            data.push_back( val );
        }
    }
    return data;
}

template<typename Adapter>
void
BenchHeap( benchmark::State& state )
{
    auto data = generateRandomData( state.range( 0 ) );
    for ( auto _ : state )
    {
        Adapter                               heap;
        std::vector<typename Adapter::Handle> handles;
        handles.reserve( data.size() );

        for ( int x : data )
        {
            handles.push_back( heap.Insert( x ) );
        }

        const size_t half = data.size() / 2;
        for ( size_t i = 0; i < half; ++i )
        {
            heap.DecreaseKey( handles[i], -static_cast<int>( i ) );

            volatile int dummy_min;
            dummy_min = heap.GetMin();
            dummy_min = heap.ExtractMin();
        }

        while ( heap.Size() > 0 )
        {
            volatile int dummy_min = heap.ExtractMin();
        }
    }
    state.SetItemsProcessed( state.iterations() * static_cast<long long>( data.size() ) );
}

template<typename Heap>
struct BoostHeapAdapter
{
    using Handle = typename Heap::handle_type;

    Handle
    Insert( int key )
    {
        return heap_.push( key );
    }

    void
    DecreaseKey( Handle h, int new_key )
    {
        heap_.decrease( h, new_key );
    }

    int
    ExtractMin()
    {
        int v = heap_.top();
        heap_.pop();
        return v;
    }

    int
    GetMin()
    {
        return heap_.top();
    }

    size_t
    Size() const
    {
        return heap_.size();
    }

  private:
    Heap heap_;
};

struct CustomFibonacciHeap
{
    using Handle = fhp::FHeap::Handle;

    Handle
    Insert( int key )
    {
        return heap_.Insert( key );
    }

    void
    DecreaseKey( Handle h, int new_key )
    {
        heap_.DecreaseKey( h, new_key );
    }

    int
    ExtractMin()
    {
        return heap_.ExtractMin();
    }

    int
    GetMin()
    {
        return heap_.GetMin();
    }

    size_t
    Size() const
    {
        return heap_.GetSize();
    }

  private:
    fhp::FHeap heap_;
};

static void
CustomArguments( benchmark::internal::Benchmark* b )
{
    const std::vector<int> sizes = { 128,     256,     384,     512,     640,     768,     896,
                                     1024,    2000,    4000,    6000,    8000,    10000,   20000,
                                     40000,   60000,   80000,   100000,  120000,  140000,  160000,
                                     250000,  500000,  750000,  1000000, 2000000, 3000000, 4000000,
                                     5000000, 6000000, 7000000, 8000000, 9000000 };
    for ( int n : sizes )
    {
        b->Arg( n );
    }
}

// clang-format off
using BoostFibonacciHeap = BoostHeapAdapter<boost::heap::fibonacci_heap<int>>;
using BoostBinomialHeap  = BoostHeapAdapter<boost::heap::binomial_heap<int>>;
using BoostBinaryHeap    = BoostHeapAdapter<boost::heap::d_ary_heap<int, boost::heap::arity<2>, boost::heap::mutable_<true>>>;
// clang-format on

BENCHMARK_TEMPLATE( BenchHeap, BoostFibonacciHeap )->Apply( CustomArguments );
BENCHMARK_TEMPLATE( BenchHeap, BoostBinomialHeap )->Apply( CustomArguments );
BENCHMARK_TEMPLATE( BenchHeap, BoostBinaryHeap )->Apply( CustomArguments );
BENCHMARK_TEMPLATE( BenchHeap, CustomFibonacciHeap )->Apply( CustomArguments );

BENCHMARK_MAIN();
