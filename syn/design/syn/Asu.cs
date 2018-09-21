namespace syn
{
    using System;
    using System.Collections.Generic;

    public struct UserDataBlock
    {
        public int size;
        public byte[] data;

        public UserDataBlock(int s, byte[] d)
        {
            size = s;
            data = d;
        }
    }

    class Asu
    {
        private static Dictionary<int, AdapterInfo> adapterMap = new Dictionary<int, AdapterInfo>();

        private List<AdapterData> adapters { get; set; }

        private List<byte[]> sequences { get; set; }

        private Dictionary<int, UserDataBlock> userDataBlockMap { get; set; }

        private Player player;

        public static void RegisterAdapter(AdapterInfo info)
        {
            adapterMap.Add(info.id, info);
        }

        public static AdapterInfo GetAdapterInfo(int id)
        {
            return adapterMap.ContainsKey(id) ? adapterMap[id] : null;
        }

        public Asu()
        {
            adapters = new List<AdapterData>();
            sequences = new List<byte[]>();
            userDataBlockMap = new Dictionary<int, UserDataBlock>();
            player = null;
        }

        public int AddUserDataBlock(int size, byte[] data) {
            // use map to reference data blocks index-independenly
            var udbId = this.userDataBlockMap.Count;
            var udb = new UserDataBlock(size, data);
            this.userDataBlockMap.Add(udbId, udb);
            return udbId;
        }

        public AdapterData AddAdapter(int id, int udbId) {
            if (adapterMap.ContainsKey(id))
            {
                var adapterInfo = adapterMap[id];
                AdapterData adapterData = new AdapterData(adapterInfo, udbId);
                this.adapters.Add(adapterData);
                return adapterData;
            }

            return null;
        }

        public int ImportFromXm(string path)
        {
            return 0;
        }

        public int Prepare()
        {
            // add user data blocks
            foreach (var usb in userDataBlockMap.Values)
            {
                this.player.userDataBlocks.Add(usb.data);
            }

            // initialize adapters, add targets
            foreach (var adapterData in adapters)
            {
                var adapter = adapterData.info.Create();
                var udb = userDataBlockMap[adapterData.udbId];
                adapter.Initialize(udb.data);
                adapter.CreateTargets(this.player);
            }

            // add sequences
            foreach (var sequence in sequences)
            {
                this.player.sequences.Add(sequence);
            }

            return 0;
        }

        public void Run(int startFrame = 0)
        {
            // this.player.sequences[0].run(startFrame);
        }

    }
}
