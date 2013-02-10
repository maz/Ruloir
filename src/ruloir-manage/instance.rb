require 'socket'
require 'fileutils'
require 'digest/crc32'

module Ruloir
  class ProgramError < Exception
  end
  
  class Instance
    public
    attr_reader :ip,:port,:token
    
    def initialize hsh
      @ip=hsh[:ip]
      @port=hsh[:port]
      @token=hsh[:token]
    end
    
    def force_update_key(a,b=nil)
      socket do |sock|
        auth sock
        sock<<"RELOAD-CACHE\r\n"
        sock<<a.to_s
        sock<<"\r\n"
        sock<<b.to_s if b
        sock<<"\r\n"
      end
    end
    
    def load_program(dir=nil)
      socket do |sock|
        auth sock
        sock<<"LOAD-PROGRAM\r\n"
        system_id=gets sock
        pgrm= block_given? ? yield(system_id) : File.join(dir,system_id)
        if pgrm.is_a?(String)
          pgrm=File.open(pgrm,"rb")
        end
        return sock<<"0\r\n" if not pgrm
        sock<<pgrm.size.to_s
        sock<<"\r\n"
        crc=Digest::CRC32.new
        while not pgrm.eof?
          #this way we can stream programs
          buf=pgrm.read(1024)
          sock<<buf
          crc<<buf
        end
        sock<<[crc.checksum].pack('N')
        status=gets sock
        msg=gets sock
        if status!='OK'
          raise ProgramError.new "#{status}: #{msg}"
        end
      end
    end
    
    private
    def gets(sock)
      str=sock.gets("\r\n").strip
    end
    def auth(sock)
      sock<<"\5"
      sock<<@token
      sock<<"\r\n"
    end
    def socket(&block)
      begin
        sock=TCPSocket.new @ip,@port
        yield sock
      ensure
        sock.close
      end
    end
  end
end