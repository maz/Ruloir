# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'ruloir/version'

Gem::Specification.new do |spec|
  spec.name          = "ruloir"
  spec.version       = Ruloir::VERSION
  spec.authors       = ["Marc Rosen"]
  spec.email         = ["ma.mazmaz@gmail.com"]
  spec.description   = %q{This gem allows you to manage a Ruloir server.}
  spec.summary       = %q{This contains various libraries to help manage a Ruloir server.}
  spec.homepage      = "https://github.com/maz/Ruloir"
  spec.license       = "MIT"

  spec.files         = `git ls-files`.split($/)
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib"]

  spec.add_dependency             "redis"
  spec.add_dependency             "digest-crc"

  spec.add_development_dependency "bundler", "~> 1.3"
  spec.add_development_dependency "rake"
end
