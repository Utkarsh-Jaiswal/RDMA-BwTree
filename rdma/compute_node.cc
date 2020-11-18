#include <iostream>
#include "rdma.h"
#include "btree.h"

void client_thread(RDMA_Manager* rdma_manager){
    std::vector<Block*> Blocks;

    Block *rootBlock = new Block();
    int msg_size = sizeof(Block);
    printf("Block Size is %d", msg_size);
//
    auto myid = std::this_thread::get_id();
    std::stringstream ss;
    ss << myid;
    std::string thread_id = ss.str();
    rdma_manager->Remote_Memory_Register(100*rdma_manager->Block_Size);
    ibv_mr* temp_mr_p;
    char* temp_char_p;
    rdma_manager->Local_Memory_Register(&temp_char_p, &temp_mr_p, 10*rdma_manager->Block_Size);
    rdma_manager->Remote_Query_Pair_Connection(thread_id);
    // file in the contend for that block.

    for (int i = 0; i<MAX; i++){
        insertNode(rootBlock, i);
    }
    // note: you need to modify the insertNode function to make
    // an RDMA data transfering every time you call this function. The insert node can help you iterate the tree
    // from the root node. you need to insert your RDMA write and read in that function.


    // Create the memory chunks for read and write. two local chunks (send and receive) and one remote chunks.
    SST_Metadata* target_sst;
    ibv_mr* send_mr;
    ibv_mr* send_map_mr;
    ibv_mr* receive_mr;
    ibv_mr* receive_map_mr;
    std::string dummyfile("dummy");
    rdma_manager->Allocate_Local_RDMA_Slot(send_mr, send_map_mr);
    rdma_manager->Allocate_Local_RDMA_Slot(receive_mr, receive_map_mr);
    rdma_manager->Allocate_Remote_RDMA_Slot(dummyfile, target_sst);
    std::cout << target_sst->mr->rkey <<std::endl;

    // Memory copy the Root node to RDMA registered buffer
    memcpy(temp_mr_p->addr, rootBlock, sizeof(Block));
    // Write to the remote memory
    rdma_manager->RDMA_Write(target_sst->mr, send_mr,
                           msg_size, thread_id);
    // Read it back to a different buffer.
    rdma_manager->RDMA_Read(target_sst->mr, receive_mr,
                          msg_size, thread_id);
    // Cast the type of the pointer to Block
    Block* root_node_readback = static_cast<Block*>(receive_mr->addr);
    // Print the Root node
    Blocks.push_back(root_node_readback);
    print(Blocks);
////  ibv_wc* wc = new ibv_wc();
//
//    rdma_manager->poll_completion(wc, 2, thread_id);
//    if (wc->status != 0){
//      fprintf(stderr, "Work completion status is %d \n", wc->status);
//      fprintf(stderr, "Work completion opcode is %d \n", wc->opcode);
//    }


//  std::cout << "write buffer: " << (char*)mem_pool_table[0].addr << std::endl;
//
//  std::cout << "read buffer: " << (char*)mem_pool_table[1].addr << std::endl;

}
int main()
{
  struct config_t config = {
      NULL,  /* dev_name */
      NULL,  /* server_name */
      19875, /* tcp_port */
      1,	 /* ib_port */ //physical
      -1, /* gid_idx */
      4*10*1024*1024 /*initial local buffer size*/
  };
  std::cout << "start" << std::endl;
  auto Remote_Bitmap = new std::unordered_map<ibv_mr*, In_Use_Array>;
  auto Local_Bitmap = new std::unordered_map<ibv_mr*, In_Use_Array>;
  RDMA_Manager* rdma_manager = new RDMA_Manager(config, Remote_Bitmap, Local_Bitmap);
//  RDMA_Manager rdma_manager(config, Remote_Bitmap, Local_Bitmap);
  rdma_manager->Client_Set_Up_Resources();
  std::thread thread_object(client_thread, rdma_manager);
  thread_object.join();

  return 0;
}
