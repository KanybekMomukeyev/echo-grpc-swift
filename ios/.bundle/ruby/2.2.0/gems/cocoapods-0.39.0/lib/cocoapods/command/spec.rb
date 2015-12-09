# encoding: utf-8

require 'active_support/core_ext/string/inflections'
require 'cocoapods/command/spec/create'
require 'cocoapods/command/spec/lint'
require 'cocoapods/command/spec/which'
require 'cocoapods/command/spec/cat'
require 'cocoapods/command/spec/edit'

module Pod
  class Command
    class Spec < Command
      self.abstract_command = true
      self.summary = 'Manage pod specs'

      #-----------------------------------------------------------------------#

      # @todo some of the following methods can probably move to one of the
      #       subclasses.

      private

      # @param [String] query the regular expression string to validate
      #
      # @raise if the query is not a valid regular expression
      #
      def validate_regex!(query)
        /#{query}/
      rescue RegexpError
        help! 'A valid regular expression is required.'
      end

      # @param  [String] spec
      #         The name of the specification.
      #
      # @param  [Bool] show_all
      #         Whether the paths for all the versions should be returned or
      #         only the one for the last version.
      #
      # @return [Pathname] the absolute path or paths of the given podspec
      #
      def get_path_of_spec(spec, show_all = false)
        sets = SourcesManager.search_by_name(spec)

        if sets.count == 1
          set = sets.first
        elsif sets.map(&:name).include?(spec)
          set = sets.find { |s| s.name == spec }
        else
          names = sets.map(&:name) * ', '
          raise Informative, "More than one spec found for '#{spec}':\n#{names}"
        end

        unless show_all
          best_spec, spec_source = spec_and_source_from_set(set)
          return pathname_from_spec(best_spec, spec_source)
        end

        all_paths_from_set(set)
      end

      # @return [Pathname] the absolute path of the given spec and source
      #
      def pathname_from_spec(spec, _source)
        Pathname(spec.defined_in_file)
      end

      # @return [String] of spec paths one on each line
      #
      def all_paths_from_set(set)
        paths = ''

        sources = set.sources

        sources.each do |source|
          versions = source.versions(set.name)

          versions.each do |version|
            spec = source.specification(set.name, version)
            paths += "#{pathname_from_spec(spec, source)}\n"
          end
        end

        paths
      end

      # @return [Specification, Source] the highest known specification with it's source of the given
      #         set.
      #
      def spec_and_source_from_set(set)
        sources = set.sources

        best_source = best_version = nil
        sources.each do |source|
          versions = source.versions(set.name)
          versions.each do |version|
            if !best_version || version > best_version
              best_source = source
              best_version = version
            end
          end
        end

        if !best_source || !best_version
          raise Informative, "Unable to locate highest known specification for `#{set.name}'"
        end

        [best_source.specification(set.name, best_version), best_source]
      end
    end
  end
end
