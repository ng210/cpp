namespace syn
{
    using System.Collections.Generic;

    public static class AdapterLib
    {
        // typedef List<AdapterInfo> (WINAPI* ALGAI)();
        public delegate List<AdapterInfo> ALGAI();

        private static List<IAdapter> adapters;

        public static List<AdapterInfo> GetAdapterInfos()
        {
            var adapterInfos = new List<AdapterInfo>();
            foreach (var adapter in adapters)
            {
                adapterInfos.Add(adapter.GetInfo());
            }
            return adapterInfos;
        }

        public static int DllMain()
        {
            // prepare adapters list
            adapters.Add(new SynthAdapter());
            return 0;
        }
    }
}
