require 'digest/md5'

module Ruloir
  class Template
    DYNAMIC_REGEX_GROUPS=/<%=[ \t\r\n]*([A-Za-z_$0-9@!]+)[ \t\r\n]*(\[[ \t\r\n]*([A-Za-z_$0-9@!]+)[ \t\r\n]*\])?[ \t\r\n]*%>/m
    DYNAMIC_REGEX_NO_GROUPS=/<%=[ \t\r\n]*[A-Za-z_$0-9@!]+[ \t\r\n]*\[[ \t\r\n]*[A-Za-z_$0-9@!]+[ \t\r\n]*\]?[ \t\r\n]*%>/m 
    
    def self.parse_file(file)
      self.parse(File.read(file), file.split('/').last)
    end
    def self.parse(code, name)
      self.new(
        code.split(DYNAMIC_REGEX_NO_GROUPS),
        code.scan(DYNAMIC_REGEX_GROUPS).map {|x|
          (x.last==x.first) ? [x.first] : [x.first, x.last]
        },
        name
      )
    end
    
    def initialize(static_chunks, dynamic_chunks, name)
      @static_chunks=static_chunks
      @dynamic_chunks=dynamic_chunks
      @name=name
    end
    
    attr_reader :name, :static_chunks, :dynamic_chunks
    
    def upload_static_chunks_to_redis(redis)
      each_static_chunk do |keys, value|
        redis.hset(keys.first, keys.last, value)
      end
    end
    
    def each_static_chunk(&block)
      i=0
      combined_chunks do |elem|
        if !elem.is_a?(Array)
          yield([key_a, i.to_s],elem)
        end
        i+=1
      end
    end
    
    def c_code
      code="int len; const char* data;\n"
      
      digest=chunk_digest
      
      i=0
      combined_chunks do |elem|
        code<< "ChunkGet(cache, "
        if elem.is_a?(Array)
          if elem.length==1
            code<< string_escape(elem.first)
            code<< ", NULL"
          else
            code<< elem.first
            code<< ", "
            code<< string_escape(elem.last)
          end
        else
          code<< string_escape(key_a)
          code<< ", #{string_escape(i)}"
        end
        i+=1
        code<< ", &data, &len);\nBufferedWrite(handle, data, len);\n"
      end
      
      code
    end
    private
    def key_a
      "#{name}_static_#{digest}"
    end
    def string_escape(x)
      x.to_s.inspect
    end
    def chunk_digest
      digest=Digest::MD5.new
      combined_chunks do |elem|
        if elem.is_a?(Array)
          #dynamic chunk
          digest<< elem.first
          digest<< "[#{elem.last}]" if elem.last!=elem.first
        else
          #static chunk
        end
      end
      digest.hexdigest
    end
    def combined_chunks(&block)
      (0..(@static_chunks.length+@dynamic_chunks.length-1)).each do |i|
        x=(((i&1)==0) ? @static_chunks : @dynamic_chunks)[i/2]
        yield(x) unless x==""
      end
    end
  end
end