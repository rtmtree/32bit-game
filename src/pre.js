(function(){
  if (typeof self === 'undefined' && typeof window !== 'undefined') self = window;
  if (typeof self.crypto === 'undefined') self.crypto = {};
  if (typeof self.crypto.getRandomValues !== 'function') {
    self.crypto.getRandomValues = function(buf) {
      for (var i = 0; i < buf.length; i++) buf[i] = (Math.random() * 256) | 0;
      return buf;
    };
  }
  if (typeof self.crypto.randomUUID !== 'function') {
    self.crypto.randomUUID = function() {
      var b = new Uint8Array(16);
      self.crypto.getRandomValues(b);
      b[6] = (b[6] & 0x0f) | 0x40; // version 4
      b[8] = (b[8] & 0x3f) | 0x80; // variant 10
      var hex = Array.prototype.map.call(b, function(x){ return (x + 0x100).toString(16).slice(1); });
      return hex[0]+hex[1]+hex[2]+hex[3]+"-"+hex[4]+hex[5]+"-"+hex[6]+hex[7]+"-"+hex[8]+hex[9]+"-"+hex[10]+hex[11]+hex[12]+hex[13]+hex[14]+hex[15];
    };
  }
})();


