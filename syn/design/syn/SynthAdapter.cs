namespace syn
{
    using System;
    using System.Collections.Generic;

    public class SynthAdapter : IAdapter
    {
        private AdapterInfo adapterInfo = null;

        public static int Id = 0x1204;

        private List<IDisposable> synths;

        public AdapterInfo GetInfo()
        {
            if (this.adapterInfo == null)
            {
                this.adapterInfo = new AdapterInfo();
            }

            return (AdapterInfo)this.adapterInfo;
        }

        public int Initialize(byte[] data)
        {
            // create sound bank
            return 0;
        }

        public int CreateTargets(Player player)
        {
            // create synths
            var synth = new object();
            this.synths.Add((IDisposable)synth);
            player.targets.Add(new Target(this, synth));
            return 0;
        }

        public void Destroy()
        {
            foreach (var synth in synths)
            {
                // delete synth
                synth.Dispose();
            }
        }
    }
}
