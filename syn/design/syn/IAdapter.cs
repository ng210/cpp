namespace syn
{
    using System;

    public class AdapterInfo
    {
        public delegate IAdapter create();

        public int id;
        public create Create;
    }

    public class AdapterData
    {
        public AdapterInfo info;
        public int udbId;

        public AdapterData(AdapterInfo info, int id)
        {
            this.info = info;
            this.udbId = id;
        }
    }

    public interface IAdapter
    {
        AdapterInfo GetInfo();

        int Initialize(byte[] data);
        int CreateTargets(Player player);
        void Destroy();
    }
}
