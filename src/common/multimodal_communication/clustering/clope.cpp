#include "clope.hpp"

#include <cmath>

namespace mc::clustering::algorithm {

static double delta_add(cluster_t& cluster, const entry_t& transaction,
                        double r) {
    size_t s = cluster.square + transaction.ones();
    size_t w = cluster.unique_item_count;
    for (size_t i{0}; i < transaction.size(); i++) {
        if (cluster.items_occurence.size() <= i) {
            cluster.items_occurence.resize(i + 1);
            cluster.items_occurence[i] = 0;
        }
        if (transaction.check_bit(i) && !cluster.items_occurence[i]) {
            w++;
        }
    }
    double new_value = s * (cluster.transaction_count + 1) / std::pow(w, r);
    if (cluster.transaction_count) {
        return new_value - cluster.square * cluster.transaction_count /
                               std::pow(cluster.unique_item_count, r);
    }
    return new_value;
}

static void update_cluster(cluster_t& cluster, const entry_t& transaction) {
    cluster.transaction_count++;
    cluster.square += transaction.ones();
    for (size_t i{0}; i < transaction.size(); i++) {
        if (cluster.items_occurence.size() <= i) {
            cluster.items_occurence.resize(i + 1);
        }
        if (transaction.check_bit(i)) {
            if (!cluster.items_occurence[i]) {
                ++cluster.unique_item_count;
            }
            cluster.items_occurence[i]++;
        }
    }
}

static void rm_transaction_from_cluster(cluster_t&     cluster,
                                        const entry_t& transaction) {
    cluster.transaction_count--;
    cluster.square -= transaction.ones();
    for (size_t i{0}; i < transaction.size(); i++) {
        if (transaction.check_bit(i)) {
            cluster.items_occurence[i]--;
            if (!cluster.items_occurence[i]) {
                --cluster.unique_item_count;
            }
        }
    }
}

static size_t choose_cluster(std::vector<cluster_t>& clusters,
                             const entry_t& transaction, double r) {
    double max_delta = delta_add(clusters[0], transaction, r);
    size_t max_i{0};

    for (size_t i{1}; i < clusters.size(); i++) {
        double delta = delta_add(clusters[i], transaction, r);

        if (delta > max_delta) {
            max_delta = delta;
            max_i     = i;
        }
    }

    cluster_t new_cluster{0, 0, 0, std::vector<size_t>(transaction.size(), 0)};
    double    delta = delta_add(new_cluster, transaction, r);
    if (delta > max_delta) {
        clusters.push_back(new_cluster);
        max_i = clusters.size() - 1;
    }
    return max_i;
}


std::pair<size_t, std::vector<size_t>> clope(const case_t& transactions,
                                             double        r) {
    if (r < 1.) {
        r = 2.;
    }
    std::vector<cluster_t> clusters(
        2, cluster_t{0, 0, 0, std::vector<size_t>(transactions[0]->size(), 0)});
    std::vector<size_t> transaction_clusters(transactions.size());

    for (size_t i{0}; i < transactions.size(); i++) {
        size_t cluster_i        = choose_cluster(clusters, *transactions[i], r);
        transaction_clusters[i] = cluster_i;
        update_cluster(clusters[cluster_i], *transactions[i]);
    }

    while (true) {
        bool moved{false};
        for (size_t i{0}; i < transactions.size(); i++) {
            rm_transaction_from_cluster(clusters[transaction_clusters[i]],
                                        *transactions[i]);
            size_t cluster_i = choose_cluster(clusters, *transactions[i], r);
            if (transaction_clusters[i] != cluster_i) {
                moved = true;
            }
            update_cluster(clusters[cluster_i], *transactions[i]);
            transaction_clusters[i] = cluster_i;
        }
        if (!moved) {
            break;
        }
    }

    for (size_t i{0}; i < clusters.size();) {
        if (!clusters[i].transaction_count) {
            clusters.erase(clusters.begin() + i);
            for (auto& j : transaction_clusters) {
                if (j > i) {
                    --j;
                }
            }
        }
        i++;
    }

    return std::make_pair(clusters.size(), transaction_clusters);
}

}  // namespace mc::clustering::algorithm